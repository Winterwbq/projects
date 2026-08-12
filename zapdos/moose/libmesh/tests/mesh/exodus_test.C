#include "mesh_elem_test.h"

#ifdef LIBMESH_HAVE_EXODUS_API

#include "libmesh/enum_to_string.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/mesh_communication.h"
#include "libmesh/mesh_serializer.h"

using namespace libMesh;

template <ElemType elem_type>
class ExodusTest : public MeshPerElemTest<elem_type>
{
public:

  void test_read_gold()
  {
    LOG_UNIT_TEST;

    Mesh input_mesh(*TestCommWorld);

    ExodusII_IO exii(input_mesh);
    if (input_mesh.processor_id() == 0)
      exii.read("meshes/exodus_elements/read_exodus_" +
                Utility::enum_to_string(elem_type) + ".e");

    MeshCommunication().broadcast(input_mesh);
    input_mesh.prepare_for_use();

    CPPUNIT_ASSERT(this->meshes_equal_enough(input_mesh, true));
  }

  void test_write()
  {
    LOG_UNIT_TEST;

    // This is a *buffered* write; we use scope to make sure the
    // ExodusII_IO object gets destructed (and thus is guaranteed to
    // finish writing and close the file) before we try to read what
    // was written.
    {
      ExodusII_IO exii(*this->_mesh);

      // We still default to 32-char names for backwards
      // compatibility, but we're writing a mesh with extra-long names
      // in it for testing, so we manually enable longer names.
      exii.set_max_name_length(80);

      exii.write("write_exodus_" +
                 Utility::enum_to_string(elem_type) + ".e");
    }

    Mesh input_mesh(*TestCommWorld);
    ExodusII_IO exii_input(input_mesh);
    if (input_mesh.processor_id() == 0)
      exii_input.read("write_exodus_" +
                      Utility::enum_to_string(elem_type) + ".e");

    MeshCommunication().broadcast(input_mesh);
    input_mesh.prepare_for_use();

    CPPUNIT_ASSERT(this->meshes_equal_enough(input_mesh, true));
  }
};

#define EXODUSTEST                              \
  CPPUNIT_TEST( test_read_gold );               \
  CPPUNIT_TEST( test_write );

#define INSTANTIATE_EXODUSTEST(elemtype)                        \
  class ExodusTest_##elemtype : public ExodusTest<elemtype> {   \
  public:                                                       \
  ExodusTest_##elemtype() :                                     \
    ExodusTest<elemtype>() {                                    \
    if (unitlog->summarized_logs_enabled())                     \
      this->libmesh_suite_name = "ExodusTest";                  \
    else                                                        \
      this->libmesh_suite_name = "ExodusTest_" #elemtype;       \
  }                                                             \
  CPPUNIT_TEST_SUITE( ExodusTest_##elemtype );                  \
  EXODUSTEST;                                                   \
  CPPUNIT_TEST_SUITE_END();                                     \
  };                                                            \
                                                                \
  CPPUNIT_TEST_SUITE_REGISTRATION( ExodusTest_##elemtype )

INSTANTIATE_EXODUSTEST(EDGE2);
INSTANTIATE_EXODUSTEST(EDGE3);
INSTANTIATE_EXODUSTEST(EDGE4);

#if LIBMESH_DIM > 1
INSTANTIATE_EXODUSTEST(TRI3);
INSTANTIATE_EXODUSTEST(TRISHELL3);
INSTANTIATE_EXODUSTEST(TRI6);
INSTANTIATE_EXODUSTEST(TRI7);

INSTANTIATE_EXODUSTEST(QUAD4);
INSTANTIATE_EXODUSTEST(QUADSHELL4);
INSTANTIATE_EXODUSTEST(QUAD8);
INSTANTIATE_EXODUSTEST(QUADSHELL8);
INSTANTIATE_EXODUSTEST(QUAD9);
INSTANTIATE_EXODUSTEST(QUADSHELL9);
#endif // LIBMESH_DIM > 1

#if LIBMESH_DIM > 2
INSTANTIATE_EXODUSTEST(TET4);
INSTANTIATE_EXODUSTEST(TET10);
INSTANTIATE_EXODUSTEST(TET14);

INSTANTIATE_EXODUSTEST(HEX8);
INSTANTIATE_EXODUSTEST(HEX20);
INSTANTIATE_EXODUSTEST(HEX27);

INSTANTIATE_EXODUSTEST(PRISM6);
INSTANTIATE_EXODUSTEST(PRISM15);
INSTANTIATE_EXODUSTEST(PRISM18);
INSTANTIATE_EXODUSTEST(PRISM20);
INSTANTIATE_EXODUSTEST(PRISM21);

// These tests use PointLocator, which uses contains_point(), which
// uses inverse_map(), which doesn't play nicely on Pyramids unless we
// have exceptions support
#ifdef LIBMESH_ENABLE_EXCEPTIONS
INSTANTIATE_EXODUSTEST(PYRAMID5);
INSTANTIATE_EXODUSTEST(PYRAMID13);
INSTANTIATE_EXODUSTEST(PYRAMID14);
INSTANTIATE_EXODUSTEST(PYRAMID18);
#endif
#endif // LIBMESH_DIM > 2

#endif // LIBMESH_HAVE_EXODUS_API
