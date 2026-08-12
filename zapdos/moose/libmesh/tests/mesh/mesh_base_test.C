
#include <libmesh/distributed_mesh.h>
#include <libmesh/elem.h>
#include <libmesh/mesh.h>
#include <libmesh/mesh_generation.h>
#include <libmesh/mesh_refinement.h>
#include <libmesh/mesh_tools.h>
#include <libmesh/replicated_mesh.h>

#include "test_comm.h"
#include "libmesh_cppunit.h"

using namespace libMesh;

class MeshBaseTest : public CppUnit::TestCase {
  /**
   * This test verifies various operations on the MeshBase and derived
   * classes.
   */
public:
  LIBMESH_CPPUNIT_TEST_SUITE( MeshBaseTest );

/* Tests need a 2d mesh */
#if LIBMESH_DIM > 1
  CPPUNIT_TEST( testDistributedMeshVerifyHasNeighborPtrs );
  CPPUNIT_TEST( testMeshVerifyHasNeighborPtrs );
  CPPUNIT_TEST( testReplicatedMeshVerifyHasNeighborPtrs );
  CPPUNIT_TEST( testDistributedMeshVerifyHasCachedElemData );
  CPPUNIT_TEST( testMeshVerifyHasCachedElemData );
  CPPUNIT_TEST( testReplicatedMeshVerifyHasCachedElemData );
  CPPUNIT_TEST( testDistributedMeshVerifyRemovalPreparation );
  CPPUNIT_TEST( testMeshVerifyRemovalPreparation );
  CPPUNIT_TEST( testReplicatedMeshVerifyRemovalPreparation );
  CPPUNIT_TEST( testDistributedMeshVerifyIsPrepared );
  CPPUNIT_TEST( testMeshVerifyIsPrepared );
  CPPUNIT_TEST( testReplicatedMeshVerifyIsPrepared );
#endif

  CPPUNIT_TEST_SUITE_END();

public:

  void setUp() {}

  void tearDown() {}

  void BrokenNeighborMesh(UnstructuredMesh & mesh)
  {
    /**
     * Build a 2d 2x2 square mesh (mesh_one) covering [0.0, 1.0] x [0.0, 1.0]
     * with linear Quad elements.
     */
    MeshTools::Generation::build_square(mesh,
                                        2, 2,
                                        0., 1.,
                                        0., 1.,
                                        QUAD9);

    // Break some neighbor links.  Of course nobody would do this in
    // real life, right?
    Elem * elem0 = mesh.query_elem_ptr(0);
    if (elem0)
      for (auto n : elem0->side_index_range())
        {
          Elem * neigh = elem0->neighbor_ptr(n);
          if (!neigh)
            continue;
          auto n_neigh = neigh->which_neighbor_am_i(elem0);
          CPPUNIT_ASSERT(n_neigh != libMesh::invalid_uint);

          elem0->set_neighbor(n, nullptr);
          neigh->set_neighbor(n_neigh, nullptr);
        }
  }

  void testMeshBaseVerifyHasNeighborPtrs(UnstructuredMesh & mesh)
  {
    this->BrokenNeighborMesh(mesh);
    mesh.unset_has_neighbor_ptrs();
    mesh.complete_preparation();
    CPPUNIT_ASSERT(mesh.is_prepared());
    CPPUNIT_ASSERT(MeshTools::valid_is_prepared(mesh));
  }

  void testDistributedMeshVerifyHasNeighborPtrs ()
  {
    DistributedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasNeighborPtrs(mesh);
  }

  void testMeshVerifyHasNeighborPtrs ()
  {
    Mesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasNeighborPtrs(mesh);
  }

  void testReplicatedMeshVerifyHasNeighborPtrs ()
  {
    ReplicatedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasNeighborPtrs(mesh);
  }

  void testMeshBaseVerifyHasCachedElemData(UnstructuredMesh & mesh)
  {
    /**
     * Build a 2d 2x2 square mesh (mesh_one) covering [0.0, 1.0] x [0.0, 1.0]
     * with linear Quad elements.
     */
    MeshTools::Generation::build_square(mesh,
                                        2, 2,
                                        0., 1.,
                                        0., 1.,
                                        QUAD9);

    // Invalidate the subdomain ids cache
    Elem * elem0 = mesh.query_elem_ptr(0);
    if (elem0)
      elem0->subdomain_id() = 1;

    // We're unprepared (prepare_for_use() will update that cache) but
    // we're not marked that way.
    CPPUNIT_ASSERT(!MeshTools::valid_is_prepared(mesh));

    mesh.unset_has_cached_elem_data();
    mesh.complete_preparation();
    CPPUNIT_ASSERT(mesh.is_prepared());
    CPPUNIT_ASSERT(MeshTools::valid_is_prepared(mesh));
  }

  void testDistributedMeshVerifyHasCachedElemData ()
  {
    DistributedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasCachedElemData(mesh);
  }

  void testMeshVerifyHasCachedElemData ()
  {
    Mesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasCachedElemData(mesh);
  }

  void testReplicatedMeshVerifyHasCachedElemData ()
  {
    ReplicatedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyHasCachedElemData(mesh);
  }

  void testMeshBaseVerifyRemovalPreparation(UnstructuredMesh & mesh)
  {
    /**
     * Build a 2d 2x2 square mesh (mesh_one) covering [0.0, 1.0] x [0.0, 1.0]
     * with linear Quad elements.
     */
    MeshTools::Generation::build_square(mesh,
                                        2, 2,
                                        0., 1.,
                                        0., 1.,
                                        QUAD9);

    // Remove elements on one side, orphaning 4 nodes and removing one
    // boundary condition.  Remove dangling neighbor pointers too; we
    // can't even clone a mesh with dangling pointers.
    for (auto & elem : mesh.element_ptr_range())
      if (elem->vertex_average()(0) > 0.5)
        mesh.delete_elem(elem);
      else
        elem->set_neighbor(1, nullptr);

    // We're unprepared (prepare_for_use() will remove those orphaned
    // nodes and fix the boundary id sets and fix the partitioning of
    // nodes that might need new owners) but we're not marked that
    // way.
    CPPUNIT_ASSERT(!MeshTools::valid_is_prepared(mesh));

    mesh.unset_is_partitioned();
    mesh.unset_has_removed_orphaned_nodes();
    mesh.unset_has_boundary_id_sets();
    mesh.complete_preparation();
    CPPUNIT_ASSERT(mesh.is_prepared());
    CPPUNIT_ASSERT(MeshTools::valid_is_prepared(mesh));
  }

  void testDistributedMeshVerifyRemovalPreparation ()
  {
    DistributedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyRemovalPreparation(mesh);
  }

  void testMeshVerifyRemovalPreparation ()
  {
    Mesh mesh(*TestCommWorld);
    testMeshBaseVerifyRemovalPreparation(mesh);
  }

  void testReplicatedMeshVerifyRemovalPreparation ()
  {
    ReplicatedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyRemovalPreparation(mesh);
  }

  void testMeshBaseVerifyIsPrepared(UnstructuredMesh & mesh)
  {
    this->BrokenNeighborMesh(mesh);

    // We're unprepared (prepare_for_use() will restitch those
    // neighbor pointers) but we're not marked that way.
    CPPUNIT_ASSERT(!MeshTools::valid_is_prepared(mesh));
  }

  void testDistributedMeshVerifyIsPrepared ()
  {
    DistributedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyIsPrepared(mesh);
  }

  void testMeshVerifyIsPrepared ()
  {
    Mesh mesh(*TestCommWorld);
    testMeshBaseVerifyIsPrepared(mesh);
  }

  void testReplicatedMeshVerifyIsPrepared ()
  {
    ReplicatedMesh mesh(*TestCommWorld);
    testMeshBaseVerifyIsPrepared(mesh);
  }
}; // End definition of class MeshBaseTest

CPPUNIT_TEST_SUITE_REGISTRATION( MeshBaseTest );
