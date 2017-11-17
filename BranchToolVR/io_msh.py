# This file can be used as a stand-alone plugin
# but here it is used in the Blened2SOFA package
bl_info = {
    'name': "GMSH Import/Export plugin",
    'author': "Saleh Dindar",
    'version': (0, 1, 0),
    'blender': (2, 69, 0),
    'location': "https://bitbucket.org/surflab/blender2sofa/src/default/io_msh.py",
    'warning': "",
    'description': "Import/export tetrahedral meshes from/to GMSH files",
    'wiki_url': "https://bitbucket.org/surflab/blender2sofa/wiki/IO_MSH",
    'tracker_url': "https://bitbucket.org/surflab/blender2sofa/issues",
    'category': "Import-Export",
}

import bpy
import bpy_extras
import re
import os
import bmesh

def encodeTriFacet(a, b, c):
  if a < b:
    if a < c:
      i,j,k = a,b,c
    else:
      i,j,k = c,a,b
  else:
    if b < c:
      i,j,k = b,c,a
    else:
      i,j,k = c,a,b

  return i << 40 | j << 20 | k
def decodeTriFacet(f):
  a = f >> 40 & ( (1 << 20) - 1 )
  b = f >> 20 & ( (1 << 20) - 1 )
  c = f       & ( (1 << 20) - 1 )
  return [int(a),int(b),int(c)]


def encodeQuadFacet(a, b, c, d):
  # rearrange the vertices so that: the first is the smallest, orientation is preserved
  '''
  vt = [a,b,c,d]
  vf = map(float,[a,b,c,d])
  mv = min(vf)
  minIdx = -1
  for vi,v in enumerate(vt):
    if abs(mv-v)<.1:
        minIdx = vi; break
  if minIdx == 0:
    i,j,k,l = a,b,c,d
  elif minIdx == 1:
    i,j,k,l = b,c,d,a
  elif minIdx == 2:
    i,j,k,l = c,d,a,b
  elif minIdx == 3:
    i,j,k,l = d,a,b,c
  # assert(max(vf) < 32768) # max index < 2^15
  return i << 45 | j << 30 | k << 15 | l
  '''
  return str(a) + " " + str(b) + " " + str(c) + " " + str(d)

def decodeQuadFacet(f):
  '''
  a = f >> 45 & ( (1 << 15) - 1 )
  b = f >> 30 & ( (1 << 15) - 1 )
  c = f >> 15 & ( (1 << 15) - 1 )
  d = f       & ( (1 << 15) - 1 )
  return [int(a),int(b),int(c),int(d)]
  '''
  strarr = f.split()
  return [int(strarr[0]), int(strarr[1]), int(strarr[2]), int(strarr[3])]

hex_faces = [ [0,1,2,3],[4,7,6,5],[0,4,5,1],[1,5,6,2],[3,2,6,7],[0,3,7,4] ]
#hex_faces = [ [0,1,5,4],[2,6,7,3],[0,2,3,1],[1,3,7,5],[4,5,7,6],[0,4,6,2] ]
tet_faces = [ [1,3,2], [0,2,3], [0,3,1], [0,1,2] ]
def recalc_outer_surface(M):
  print('recalculating outer surface')
  triFaceSet = set()
  for t in M.tetrahedra:
    for l in tet_faces:
      f = encodeTriFacet(t.vertices[l[0]],t.vertices[l[1]],t.vertices[l[2]])
      rf = encodeTriFacet(t.vertices[l[0]],t.vertices[l[2]],t.vertices[l[1]])
      if rf in triFaceSet:
        triFaceSet.remove(rf)
      else:
        triFaceSet.add(f)

  quadFaceSet = set()
  for t in M.hexahedra:
    for l in hex_faces:
      f = encodeQuadFacet(t.vertices[l[0]],t.vertices[l[1]],t.vertices[l[2]],t.vertices[l[3]])
      rf = encodeQuadFacet(t.vertices[l[0]],t.vertices[l[3]],t.vertices[l[2]],t.vertices[l[1]])
      if rf in quadFaceSet:
        quadFaceSet.remove(rf)
      else:
        quadFaceSet.add(f)

  bm = bmesh.new()
  bm.from_mesh(M)

  # Remove all the faces created previously,
  # since we are recalculating the outer surface
  for f in bm.faces:
    bm.faces.remove(f)
  for e in bm.edges:
    bm.edges.remove(e)

  if hasattr(bm.verts, 'ensure_lookup_table'):
    bm.verts.ensure_lookup_table()
    bm.edges.ensure_lookup_table()
    bm.faces.ensure_lookup_table()

  # Add all the triangular faces
  for f in triFaceSet:
    a, b, c = decodeTriFacet(f)
    bm.faces.new([bm.verts[a],bm.verts[b],bm.verts[c]])

  # Add all the quad faces
  count = 0
  for f in quadFaceSet:
    # for some reason we have to reverse the faces
    # All the logic here is sound but without inverting
    # the order the surfaces look inside out
    c, b, a, d = decodeQuadFacet(f)
    #a, b, c, d = decodeQuadFacet(f)
    #print(bm.verts[a])
    #print(bm.verts[b])
    #print(bm.verts[c])
    #print(bm.verts[d])
    bm.faces.new([bm.verts[a],bm.verts[b],bm.verts[c],bm.verts[d]])
    #print('created new face %d' % count)
    count += 1

  # Update the data structures
  bm.faces.index_update()
  bm.to_mesh(M)
  bm.free()
  M.update(calc_edges=True)
  M.calc_normals()

class MeshTetrahedron(bpy.types.PropertyGroup):
    """Represent a tetrahedron in a mesh"""
    vertices = bpy.props.IntVectorProperty(size=4)

class MeshHexahedron(bpy.types.PropertyGroup):
    """Represent a hexahedron in a mesh"""
    vertices = bpy.props.IntVectorProperty(size=8)

# A panel will give us a permanent place in the data tab for meshes
class VolumetricMeshPanel(bpy.types.Panel):
    """A panel to edit tetrahedral and hexahedral mesh properties"""
    bl_label = "Volumetric Mesh"
    bl_idname = "MESH_PT_Volumetric"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "data"

    @classmethod
    def poll(self, context):
        return (context.object is not None and context.object.type == 'MESH')

    def check(self, context):
        return True
    def draw(self, context):
        layout = self.layout
        M = context.object.data
        layout.label("Tetrahedron count: %d" % len(M.tetrahedra))
        layout.label("Hexahedron count: %d" % len(M.hexahedra))
        layout.operator('mesh.recalculate_outer_surface', icon='RETOPO')
        layout.operator('mesh.remove_degenerate_hexa')

class ReCalculateOuterSurface(bpy.types.Operator):
  bl_idname = "mesh.recalculate_outer_surface"
  bl_label = "Recalculate outer surfaces of a volumetric mesh"
  bl_description = "Remove all the polygons of the mesh and re-calcualte the outer surface by going through all tetrahedra and hexahedra"

  @classmethod
  def poll(cls, context):
    o = context.object
    return o is not None and o.type == 'MESH' and len(o.data.tetrahedra) + len(o.data.hexahedra) > 0

  def execute(self, context):
    recalc_outer_surface(context.object.data)
    return { 'FINISHED' }

def remove_degenerate_hexahedra(M):
    inverted_hexa = 0
    degenerate_hexa = []
    for i, h in enumerate(M.hexahedra):
      v = []
      for j in h.vertices:
  	    v.append(M.vertices[j].co)
      a = v[1] - v[0]
      b = v[3] - v[0]
      c = v[4] - v[0]
      V = a.cross(b).dot(c)
      if (abs(V) < 0.01 * (a.length**3 + b.length**3 + c.length**3)):
  	    degenerate_hexa.append(i)
      elif V < 0.0:
        w = h.vertices
        h.vertices = [ w[4], w[5], w[6], w[7], w[0], w[1], w[2], w[3] ]
        inverted_hexa = inverted_hexa + 1

    degenerate_hexa.reverse()
    for i in degenerate_hexa:
        M.hexahedra.remove(i)
    recalc_outer_surface(M)
    return

class RemoveDegenerateHexahedra(bpy.types.Operator):
  bl_idname = "mesh.remove_degenerate_hexa"
  bl_label = "Remove degenerate hexahedra"
  bl_description = "Find all the hexahedra with very small volume and remove them, also find inverted hexahehdra and fix them"
  bl_options = { 'UNDO' }

  @classmethod
  def poll(cls, context):
    o = context.object
    return o is not None and o.type == 'MESH' and len(o.data.tetrahedra) + len(o.data.hexahedra) > 0

  def execute(self, context):
    M = context.object.data

  def execute(self, context):
    M = context.object.data

    inverted_hexa = 0
    degenerate_hexa = []
    for i, h in enumerate(M.hexahedra):
      v = []
      for j in h.vertices:
  	    v.append(M.vertices[j].co)
      a = v[1] - v[0]
      b = v[3] - v[0]
      c = v[4] - v[0]
      V = a.cross(b).dot(c)
      if (abs(V) < 0.01 * (a.length**3 + b.length**3 + c.length**3)):
  	    degenerate_hexa.append(i)
      elif V < 0.0:
        w = h.vertices
        h.vertices = [ w[4], w[5], w[6], w[7], w[0], w[1], w[2], w[3] ]
        inverted_hexa = inverted_hexa + 1

    degenerate_hexa.reverse()
    for i in degenerate_hexa:
        M.hexahedra.remove(i)
    self.report({ 'INFO' }, "Removed %d degenerate hexahedra and inverted %d hexahedra" % (len(degenerate_hexa),inverted_hexa))
    recalc_outer_surface(M)
    return { 'FINISHED' }

class ExportMSHOperator(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
  bl_idname = "export_mesh.msh"
  bl_label = "Export MSH"

  filename_ext = ".msh"
  filter_glob = bpy.props.StringProperty(default="*.msh", options={'HIDDEN'})
  @classmethod
  def poll(cls, context):
      """
      Enabled only for tetrahedral and hexahedral meshes
      """
      o = context.object
      return o is not None and o.type == 'MESH' \
        and len(o.data.hexahedra) + len(o.data.tetrahedra) > 0

  def execute(self, context):
      self.report({'ERROR'}, "Not implemented yet")
      return { 'ERROR' }

class ImportMSH(bpy.types.Operator, bpy_extras.io_utils.ImportHelper):
  """Load a tetrahedral mesh from GMSH file, make sure to reverse the normals of the mesh if necessary"""
  bl_idname = "import_mesh.msh"
  bl_label = "Import MSH"
  bl_options = {'UNDO'}

  filename_ext = ".msh"
  filter_glob = bpy.props.StringProperty(default="*.msh", options={'HIDDEN'})

  def execute(self, context):
    objName = bpy.path.display_name(os.path.basename(self.filepath))
    M = bpy.data.meshes.new(name = objName)
    lineno = 0; mode = ''
    f = open(self.filepath, 'r')
    for line in f:
      lineno += 1
      line = line.strip()
      if mode == '':
        if line == '$NOD':
          mode = 'NOD-first'
        elif line == '$ELM':
          mode = 'ELM-first'
        else:
          self.report({'ERROR'}, "%s:%d: Unexpected %s" % (objName,line, lineno))
          return {'CANCELLED'}
      elif mode == 'NOD-first':
        if re.match('^\d+\s*$', line):
          M.vertices.add(int(line))
          mode = 'NOD'
        else:
          self.report({'ERROR'}, "%s:%d: Expected number of nodes here got '%s'" %(objName,lineno, line))
          return {'CANCELLED'}
      elif mode == 'NOD':
        if re.match('^\d+(\s+[+-]?\d+(\.\d*([eE][+-]?\d+)?)?){3}\s*$', line):
          i, x, y, z = line.split()
          M.vertices[int(i)-1].co[0] = float(x)
          M.vertices[int(i)-1].co[1] = float(y)
          M.vertices[int(i)-1].co[2] = float(z)
        elif line == '$ENDNOD':
          mode = ''
        else:
          self.report({'ERROR'}, "%s:%d: Line of node def expected here '%s'" % (objName, lineno, line))
          return {'CANCELLED'}
      elif mode == 'ELM-first':
        if re.match('^\d+\s*$', line):
          # Tthere is no way to resize the tetrahedra collection
          mode = 'ELM'
        else:
          self.report({'ERROR'}, "%s:%d: Expected number of elements here got '%s'" %(objName, lineno, line))
          return {'CANCELLED'}
      elif mode == 'ELM':
        if re.match('^\d+(\s+\d+)+\s*$', line):
          idx, count, _, _, _, a, b, c, d = line.split()
          # TODO: what about triangle elements count=3
          # TODO: what about hexahedral elements count=8
          if count == '4':   
              i = int(idx)
              t = M.tetrahedra.add()
              if i != len(M.tetrahedra):
                self.report({'ERROR'}, "%s:%d: Elements must come in proper order, %d" %(objName,lineno,i))
                return {'CANCELLED'}
              t.vertices[0] = int(a) - 1
              t.vertices[1] = int(b) - 1
              t.vertices[2] = int(c) - 1
              t.vertices[3] = int(d) - 1  
        elif line == '$ENDELM':
          mode = ''
        else:
          self.report({'ERROR'}, "%s:%d: Line of element def expected here '%s'" % (objName, lineno,line))
          return {'CANCELLED'}
      else:
        assert("This line" == "Never reached")
    f.close()
    recalc_outer_surface(M)
    o = bpy.data.objects.new(objName, M)
    context.scene.objects.link(o)
    o.select = True
    return { 'FINISHED' }





def menu_func_import(self, context):
    self.layout.operator(ImportMSH.bl_idname, text="GMSH (.msh)")

def register_other():
    bpy.types.INFO_MT_file_import.append(menu_func_import)
    bpy.types.Mesh.tetrahedra = bpy.props.CollectionProperty(name="Tetrahedra", type=MeshTetrahedron)
    bpy.types.Mesh.hexahedra = bpy.props.CollectionProperty(name="Hexahedra", type=MeshHexahedron)

def unregister_other():
    del bpy.types.Mesh.tetrahedra
    del bpy.types.Mesh.hexahedra
    bpy.types.INFO_MT_file_import.remove(menu_func_import)


def register():
    register_other()
    bpy.utils.register_module(__name__)

def unregister():
    unregister_other()
    bpy.utils.unregister_module(__name__)
