#include "mesh_elem_test.h"

#include "libmesh/enum_to_string.h"
#include "libmesh/mesh_communication.h"
#include "libmesh/xdr_io.h"

using namespace libMesh;

template <ElemType elem_type>
class XdrIOTest : public MeshPerElemTest<elem_type> {

public:

  void test_read_gold(bool binary)
  {
    LOG_UNIT_TEST;

    Mesh input_mesh(*TestCommWorld);

    XdrIO xdr_io(input_mesh);
    xdr_io.binary() = binary;
    xdr_io.read("meshes/xdrio_elements/read_xdrio_" +
                Utility::enum_to_string(elem_type) +
                (binary ? ".xdr" : ".xda"));

    input_mesh.prepare_for_use();

    CPPUNIT_ASSERT(this->meshes_equal_enough(input_mesh, false));
  }

  void test_read_gold_xda() { this->test_read_gold(false); }
  void test_read_gold_xdr() { this->test_read_gold(true); }

  void test_write(bool binary)
  {
    LOG_UNIT_TEST;

    // This is a *buffered* write; we use scope to make sure the
    // XdrIO object gets destructed (and thus is guaranteed to
    // finish writing and close the file) before we try to read what
    // was written.
    {
      XdrIO xdr_io(*this->_mesh);
      xdr_io.binary() = binary;
      xdr_io.write("write_xdrio_" +
                   Utility::enum_to_string(elem_type) +
                   (binary ? ".xdr" : ".xda"));
    }

    Mesh input_mesh(*TestCommWorld);
    XdrIO xdr_io_input(input_mesh);
    xdr_io_input.binary() = binary;
    xdr_io_input.read("write_xdrio_" +
                      Utility::enum_to_string(elem_type) +
                      (binary ? ".xdr" : ".xda"));

    MeshCommunication().broadcast(input_mesh);
    input_mesh.prepare_for_use();

    CPPUNIT_ASSERT(this->meshes_equal_enough(input_mesh, false));
  }

  void test_write_xda() { this->test_write(false); }
  void test_write_xdr() { this->test_write(true); }

};

#if LIBMESH_HAVE_XDR
#define XDRIOTEST                     \
  CPPUNIT_TEST( test_read_gold_xda ); \
  CPPUNIT_TEST( test_read_gold_xdr ); \
  CPPUNIT_TEST( test_write_xda ); \
  CPPUNIT_TEST( test_write_xdr );
#else
#define XDRIOTEST                     \
  CPPUNIT_TEST( test_read_gold_xda ); \
  CPPUNIT_TEST( test_write_xda );
#endif

#define INSTANTIATE_XDRIOTEST(elemtype)                        \
  class XdrIOTest_##elemtype : public XdrIOTest<elemtype> {   \
  public:                                                       \
  XdrIOTest_##elemtype() :                                     \
    XdrIOTest<elemtype>() {                                    \
    if (unitlog->summarized_logs_enabled())                     \
      this->libmesh_suite_name = "XdrIOTest";                  \
    else                                                        \
      this->libmesh_suite_name = "XdrIOTest_" #elemtype;       \
  }                                                             \
  CPPUNIT_TEST_SUITE( XdrIOTest_##elemtype );                  \
  XDRIOTEST;                                                   \
  CPPUNIT_TEST_SUITE_END();                                     \
  };                                                            \
                                                                \
  CPPUNIT_TEST_SUITE_REGISTRATION( XdrIOTest_##elemtype )

INSTANTIATE_XDRIOTEST(EDGE2);
INSTANTIATE_XDRIOTEST(EDGE3);
INSTANTIATE_XDRIOTEST(EDGE4);

#if LIBMESH_DIM > 1
INSTANTIATE_XDRIOTEST(TRI3);
INSTANTIATE_XDRIOTEST(TRISHELL3);
INSTANTIATE_XDRIOTEST(TRI6);
INSTANTIATE_XDRIOTEST(TRI7);

INSTANTIATE_XDRIOTEST(QUAD4);
INSTANTIATE_XDRIOTEST(QUADSHELL4);
INSTANTIATE_XDRIOTEST(QUAD8);
INSTANTIATE_XDRIOTEST(QUADSHELL8);
INSTANTIATE_XDRIOTEST(QUAD9);
INSTANTIATE_XDRIOTEST(QUADSHELL9);
#endif // LIBMESH_DIM > 1

#if LIBMESH_DIM > 2
INSTANTIATE_XDRIOTEST(TET4);
INSTANTIATE_XDRIOTEST(TET10);
INSTANTIATE_XDRIOTEST(TET14);

INSTANTIATE_XDRIOTEST(HEX8);
INSTANTIATE_XDRIOTEST(HEX20);
INSTANTIATE_XDRIOTEST(HEX27);

INSTANTIATE_XDRIOTEST(PRISM6);
INSTANTIATE_XDRIOTEST(PRISM15);
INSTANTIATE_XDRIOTEST(PRISM18);
INSTANTIATE_XDRIOTEST(PRISM20);
INSTANTIATE_XDRIOTEST(PRISM21);

// These tests use PointLocator, which uses contains_point(), which
// uses inverse_map(), which doesn't play nicely on Pyramids unless we
// have exceptions support
#ifdef LIBMESH_ENABLE_EXCEPTIONS
INSTANTIATE_XDRIOTEST(PYRAMID5);
INSTANTIATE_XDRIOTEST(PYRAMID13);
INSTANTIATE_XDRIOTEST(PYRAMID14);
INSTANTIATE_XDRIOTEST(PYRAMID18);
#endif
#endif // LIBMESH_DIM > 2
