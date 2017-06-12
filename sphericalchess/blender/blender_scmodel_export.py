
bl_info = {
    "name": "Spherical Chess model format (.scmodel)",
    "version": (0, 1),
    "blender": (2, 76, 0),
    "location": "File > Export",
    "description": "",
    "warning": "",
    "category": "Import-Export",
}


import bpy
import mathutils
import struct
import os

from bpy.props import StringProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper

def TrimEncodeName(name):
    return name[1:].encode("ascii")[:255] # 255, max size of 8-bit unsigned char

class Exporter(bpy.types.Operator, ExportHelper):
    bl_idname = "export_mesh.scmodel"
    bl_label = "Export SCMODEL"

    filename_ext = ".scmodel"
    filter_glob = StringProperty(default="*.scmodel", options={"HIDDEN"})


    def execute(self, context):
        
        scene = bpy.context.scene

        markers = []
        geometries = []
        collisions = []

        for obj in bpy.context.scene.objects:
            if obj.name[0] == '@':
                markers.append(obj)
            elif obj.name[0] == '^':
                geometries.append(obj)
            elif obj.name[0] == '%':
                collisions.append(obj)

        comparisonFunc = lambda x: x.name

        markers.sort(key = comparisonFunc)
        geometries.sort(key = comparisonFunc)
        collisions.sort(key = comparisonFunc)

        output = open(self.filepath, "wb")

        output.write(struct.pack("I", len(markers)))

        for obj in markers:

            loc, rot, scale = obj.matrix_world.decompose()
            rot = rot.to_matrix()

            name = TrimEncodeName(obj.name)
            output.write(struct.pack("B", len(name)) + name)

            output.write(struct.pack("fff", loc.x, loc.y, loc.z))
            output.write(struct.pack("fff", rot[0][0], rot[1][0], rot[2][0]))
            output.write(struct.pack("fff", rot[0][2], rot[1][2], rot[2][2]))


        output.write(struct.pack("I", len(geometries)))

        for obj in geometries:

            name = TrimEncodeName(obj.name)
            output.write(struct.pack("B", len(name)) + name)

            outputVertexCountOffset = output.tell()
            output.write(struct.pack("I", 0)) # leave space to be filled with actual value later         

            try:
                mesh = obj.to_mesh(scene, True, "PREVIEW", True)
            except:
                continue

            vertexCount = 0

            for face in mesh.tessfaces:
                
                if mesh.tessface_uv_textures:
                    layer = mesh.tessface_uv_textures.active.data
                    tex = layer[face.index]

                    coords = [ tex.uv1[:], tex.uv2[:], tex.uv3[:], tex.uv4[:] ]
                else:
                    coords = [ [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0] ]

                if len(face.vertices) == 4:
                    indices = [0, 1, 2, 2, 3, 0]
                else:
                    indices = [0, 1, 2]

                for i in indices:
                    v = face.vertices[i]

                    vertexCount += 1

                    output.write(struct.pack("fff", *(obj.matrix_world * mesh.vertices[v].co)))
                    
                    if face.use_smooth == True:
                        output.write(struct.pack("fff", *mesh.vertices[v].normal)) # todo transform by matrix_world
                    else:
                        output.write(struct.pack("fff", *face.normal)) # todo transform by matrix_world ?

                    output.write(struct.pack("ff", *coords[i]))

            bpy.data.meshes.remove(mesh) # todo is this ok? if something above exceptions this wont be called ?
            
            output.seek(outputVertexCountOffset)
            output.write(struct.pack("I", vertexCount))
            output.seek(0, os.SEEK_END)
            

        vertexCount = 0
        outputVertexCountOffset = output.tell()

        output.write(struct.pack("I", 0)) # leave space to be filled with actual value later

        for obj in collisions:

            # todo, use mesh here instead to_mesh ?

            if not obj.data.tessfaces:
                obj.data.update(calc_tessface = True)

            for p in obj.data.tessfaces:

                if len(p.vertices) == 4:
                    indices = [0, 1, 2, 2, 3, 0]
                else:
                    indices = [0, 1, 2]

                for i in indices:
                    vertexCount += 1
                    output.write(struct.pack("fff", *(obj.matrix_world * obj.data.vertices[p.vertices[i]].co)))


        output.seek(outputVertexCountOffset)
        output.write(struct.pack("I", vertexCount // 3))

        output.close()

        return {'FINISHED'}


def menu_export(self, context):
    self.layout.operator(Exporter.bl_idname, text="Spherical Chess Model (.scmodel)")


def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_export)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_export)

if __name__ == "__main__":
    register()
