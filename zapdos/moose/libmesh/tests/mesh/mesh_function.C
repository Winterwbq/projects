#include <libmesh/equation_systems.h>
#include <libmesh/replicated_mesh.h>
#include <libmesh/mesh_generation.h>
#include <libmesh/dof_map.h>
#include <libmesh/system.h>
#include <libmesh/mesh_function.h>
#include <libmesh/numeric_vector.h>
#include <libmesh/elem.h>

#include "test_comm.h"
#include "libmesh_cppunit.h"
#include "libmesh/enum_xdr_mode.h"


using namespace libMesh;

Number projection_function (const Point & p,
                            const Parameters &,
                            const std::string &,
                            const std::string &)
{
  return
    cos(.5*libMesh::pi*p(0)) *
    sin(.5*libMesh::pi*p(1)) *
    cos(.5*libMesh::pi*p(2));
}

Number trilinear_function (const Point & p,
                           const Parameters &,
                           const std::string &,
                           const std::string &)
{
  return 8*p(0) + 80*p(1) + 800*p(2);
}


Number bilinear_function (const Point & p,
                          const Parameters &,
                          const std::string &,
                          const std::string &)
{
  return 8*p(0) + 80*p(1);
}


Number biquadratic_function (const Point & p,
                             const Parameters &,
                             const std::string &,
                             const std::string &)
{
  return 7.5*p(0)*p(0) + 75*p(1)*p(1) + 0.75*p(1)*p(0);
}



class MeshFunctionTest : public CppUnit::TestCase
{
  /**
   * Tests for general MeshFunction capability.
   */
public:
  LIBMESH_CPPUNIT_TEST_SUITE( MeshFunctionTest );

#if LIBMESH_DIM > 1
  CPPUNIT_TEST( test_subdomain_id_sets );
  CPPUNIT_TEST( test_bad_gradient_var_with_out_of_mesh_value );
#ifdef LIBMESH_ENABLE_SECOND_DERIVATIVES
  CPPUNIT_TEST( test_bad_hessian_var_with_out_of_mesh_value );
#endif
#ifdef LIBMESH_HAVE_PETSC
  CPPUNIT_TEST( vectorMeshFunctionLagrange );
  CPPUNIT_TEST( vectorMeshFunctionNedelec );
  CPPUNIT_TEST( vectorMeshFunctionRaviartThomas );
  CPPUNIT_TEST( mixedScalarAndVectorVariables );
#endif // LIBMESH_HAVE_PETSC
#endif
#if LIBMESH_DIM > 2
#ifdef LIBMESH_ENABLE_AMR
  CPPUNIT_TEST( test_p_level );
#endif
#endif

  CPPUNIT_TEST_SUITE_END();

protected:

public:
  void setUp() {}

  void tearDown() {}

  // test that mesh function works correctly with subdomain id sets.
  void test_subdomain_id_sets()
  {
    LOG_UNIT_TEST;

    ReplicatedMesh mesh(*TestCommWorld);

    MeshTools::Generation::build_square (mesh,
                                         4, 4,
                                         0., 1.,
                                         0., 1.,
                                         QUAD4);

    // Set a subdomain id for all elements, based on location.
    for (auto & elem : mesh.active_element_ptr_range())
      {
        Point c = elem->vertex_average();
        elem->subdomain_id() =
          subdomain_id_type(c(0)*4) + subdomain_id_type(c(1)*4)*10;
      }

    // Add a discontinuous variable so we can easily see what side of
    // an interface we're querying
    EquationSystems es(mesh);
    System & sys = es.add_system<System> ("SimpleSystem");
    unsigned int u_var = sys.add_variable("u", CONSTANT, MONOMIAL);

    es.init();
    sys.project_solution(trilinear_function, nullptr, es.parameters);

    MeshFunction mesh_function (sys.get_equation_systems(),
                                *sys.current_local_solution,
                                sys.get_dof_map(),
                                u_var);

    // Checkerboard pattern
    const std::set<subdomain_id_type> sbdids1 {0,2,11,13,20,22,31,33};
    const std::set<subdomain_id_type> sbdids2 {1,3,10,12,21,23,30,32};
    mesh_function.init();
    mesh_function.enable_out_of_mesh_mode(DenseVector<Number>());
    mesh_function.set_subdomain_ids(&sbdids1);

    DenseVector<Number> vec_values;
    const std::string dummy;

    // Make sure the MeshFunction's values interpolate the projected solution
    // at the nodes
    for (auto & elem : mesh.active_local_element_ptr_range())
      {
        const Point c = elem->vertex_average();
        const Real expected_value =
          libmesh_real(trilinear_function(c, es.parameters, dummy, dummy));
        const std::vector<Point> offsets
          {{0,-1/8.}, {1/8.,0}, {0,1/8.}, {-1/8.,0}};
        for (Point offset : offsets)
          {
            const Point p = c + offset;
            mesh_function(p, 0, vec_values, &sbdids1);
            const Number retval1 = vec_values.empty() ? -12345 : vec_values(0);
            mesh_function(p, 0, vec_values, &sbdids2);
            const Number retval2 = vec_values.empty() ? -12345 : vec_values(0);
            mesh_function(c, 0, vec_values, nullptr);
            const Number retval3 = vec_values.empty() ? -12345 : vec_values(0);

            LIBMESH_ASSERT_NUMBERS_EQUAL
              (retval3, expected_value, TOLERANCE * TOLERANCE);

            if (sbdids1.count(elem->subdomain_id()))
              {
                CPPUNIT_ASSERT(!sbdids2.count(elem->subdomain_id()));
                LIBMESH_ASSERT_NUMBERS_EQUAL
                  (retval1, expected_value, TOLERANCE * TOLERANCE);

                mesh_function(c, 0, vec_values, &sbdids2);
                CPPUNIT_ASSERT(vec_values.empty());
              }
            else
              {
                LIBMESH_ASSERT_NUMBERS_EQUAL
                  (retval2, expected_value, TOLERANCE * TOLERANCE);

                mesh_function(c, 0, vec_values, &sbdids1);
                CPPUNIT_ASSERT(vec_values.empty());
              }
          }
      }
  }

  void test_bad_gradient_var_with_out_of_mesh_value()
  {
    LOG_UNIT_TEST;

    ReplicatedMesh mesh(*TestCommWorld);

    MeshTools::Generation::build_square(mesh,
                                        4, 4,
                                        0., 1.,
                                        0., 1.,
                                        QUAD4);

    EquationSystems es(mesh);
    System & sys = es.add_system<System>("SimpleSystem");
    const unsigned int u_var = sys.add_variable("u", FIRST, LAGRANGE);
    const unsigned int v_var = sys.add_variable("v", CONSTANT, MONOMIAL);

    es.init();
    sys.project_solution(bilinear_function, nullptr, es.parameters);

    std::vector<unsigned int> variables {u_var, v_var, libMesh::invalid_uint};
    MeshFunction mesh_function(es,
                               *sys.current_local_solution,
                               sys.get_dof_map(),
                               variables);
    mesh_function.init();

    DenseVector<Number> out_of_mesh_value(3);
    out_of_mesh_value(0) = -99.0;
    out_of_mesh_value(1) = 5.25;
    out_of_mesh_value(1) = 42;
    mesh_function.enable_out_of_mesh_mode(out_of_mesh_value);

    // Pick an element centroid so u and v agree
    const Point good_p(0.375, 0.625, 0.0);

    // On a distributed mesh not every processor may have every
    // element
    const Elem * elem = (*mesh.sub_point_locator())(good_p);
    bool someone_found_elem = elem;
    mesh.comm().max(someone_found_elem);
    CPPUNIT_ASSERT(someone_found_elem);

    std::vector<Gradient> gradients;
    mesh_function.gradient(good_p, 0.0, gradients);

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), gradients.size());

    // Let's only test our evaluation where we know we can evaluate, in parallel
    if (!elem || elem->processor_id() != mesh.processor_id())
      return;

    LIBMESH_ASSERT_NUMBERS_EQUAL(8.0, gradients[0](0), TOLERANCE * TOLERANCE);
    LIBMESH_ASSERT_NUMBERS_EQUAL(80.0, gradients[0](1), TOLERANCE * TOLERANCE);
    if (LIBMESH_DIM > 2)
      LIBMESH_ASSERT_NUMBERS_EQUAL(0.0, gradients[0](2), TOLERANCE * TOLERANCE);

    // Piecewise-constant functions have zero gradients
    for (unsigned int d = 0; d < LIBMESH_DIM; ++d)
      LIBMESH_ASSERT_NUMBERS_EQUAL(0, gradients[1](d), TOLERANCE * TOLERANCE);

    LIBMESH_ASSERT_NUMBERS_EQUAL(out_of_mesh_value(2),
                                 gradients[2](0),
                                 TOLERANCE * TOLERANCE);
    for (unsigned int d = 1; d < LIBMESH_DIM; ++d)
      LIBMESH_ASSERT_NUMBERS_EQUAL(0, gradients[2](d),
                                   TOLERANCE * TOLERANCE);

    const Point bad_p(1.375, 0.625, 0.0);
    mesh_function.gradient(bad_p, 0.0, gradients);

    for (unsigned int vn = 0; vn != 3; ++vn)
      {
        LIBMESH_ASSERT_NUMBERS_EQUAL(out_of_mesh_value(vn),
                                     gradients[vn](0),
                                     TOLERANCE * TOLERANCE);
        for (unsigned int d = 1; d < LIBMESH_DIM; ++d)
          LIBMESH_ASSERT_NUMBERS_EQUAL(0, gradients[vn](d),
                                       TOLERANCE * TOLERANCE);
      }
  }

#ifdef LIBMESH_ENABLE_SECOND_DERIVATIVES
  void test_bad_hessian_var_with_out_of_mesh_value()
  {
    LOG_UNIT_TEST;

    // Hessian calculations are a little weaker in FP on some systems
    const Real tol = TOLERANCE * std::sqrt(TOLERANCE);

    ReplicatedMesh mesh(*TestCommWorld);

    MeshTools::Generation::build_square(mesh,
                                        4, 4,
                                        0., 1.,
                                        0., 1.,
                                        QUAD9);

    EquationSystems es(mesh);
    System & sys = es.add_system<System>("SimpleSystem");
    unsigned int u_var = sys.add_variable("u", SECOND, LAGRANGE);
    unsigned int v_var = sys.add_variable("v", CONSTANT, MONOMIAL);

    es.init();
    sys.project_solution(biquadratic_function, nullptr, es.parameters);

    std::vector<unsigned int> variables {u_var, v_var, libMesh::invalid_uint};
    MeshFunction mesh_function(es,
                               *sys.current_local_solution,
                               sys.get_dof_map(),
                               variables);
    mesh_function.init();

    DenseVector<Number> out_of_mesh_value(3);
    out_of_mesh_value(0) = -99.0;
    out_of_mesh_value(1) = 5.25;
    out_of_mesh_value(2) = 42;
    mesh_function.enable_out_of_mesh_mode(out_of_mesh_value);

    std::vector<Tensor> hessians;
    const Point p(0.4, 0.6, 0.0);

    // On a distributed mesh not every processor may have every
    // element
    const Elem * elem = (*mesh.sub_point_locator())(p);
    bool someone_found_elem = elem;
    mesh.comm().max(someone_found_elem);
    CPPUNIT_ASSERT(someone_found_elem);

    mesh_function.hessian(p, 0.0, hessians);

    CPPUNIT_ASSERT_EQUAL(std::size_t(3), hessians.size());

    // Let's only test our evaluation where we know we can evaluate, in parallel
    if (!elem || elem->processor_id() != mesh.processor_id())
      return;

    LIBMESH_ASSERT_NUMBERS_EQUAL(15.0, hessians[0](0,0), tol);
    LIBMESH_ASSERT_NUMBERS_EQUAL(0.75, hessians[0](0,1), tol);
    LIBMESH_ASSERT_NUMBERS_EQUAL(0.75, hessians[0](1,0), tol);
    LIBMESH_ASSERT_NUMBERS_EQUAL(150.0, hessians[0](1,1), tol);

    if (LIBMESH_DIM > 2)
       for (unsigned int d = 0; d < LIBMESH_DIM; ++d)
         for (unsigned int d2 = 0; d2 < LIBMESH_DIM; ++d2)
           if (d>1 || d2>1)
             LIBMESH_ASSERT_NUMBERS_EQUAL(0, hessians[0](d,d2),
                                          tol);

    // Piecewise-constant functions have zero Hessians
    for (unsigned int d = 0; d < LIBMESH_DIM; ++d)
      for (unsigned int d2 = 0; d2 < LIBMESH_DIM; ++d2)
        LIBMESH_ASSERT_NUMBERS_EQUAL(0, hessians[1](d,d2), tol);

    LIBMESH_ASSERT_NUMBERS_EQUAL(out_of_mesh_value(2),
                                 hessians[2](0,0),
                                 tol);
    for (unsigned int d = 0; d < LIBMESH_DIM; ++d)
      for (unsigned int d2 = 0; d2 < LIBMESH_DIM; ++d2)
        if (d || d2)
          LIBMESH_ASSERT_NUMBERS_EQUAL(0, hessians[2](d,d2),
                                       tol);

    const Point bad_p(1.375, 0.625, 0.0);
    mesh_function.hessian(bad_p, 0.0, hessians);

    for (unsigned int vn = 0; vn != 2; ++vn)
      {
        LIBMESH_ASSERT_NUMBERS_EQUAL(out_of_mesh_value(vn),
                                     hessians[vn](0,0), tol);
        for (unsigned int d = 0; d < LIBMESH_DIM; ++d)
          for (unsigned int d2 = 0; d2 < LIBMESH_DIM; ++d2)
            if (d || d2)
              LIBMESH_ASSERT_NUMBERS_EQUAL(0, hessians[vn](d,d2), tol);
      }
  }
#endif // LIBMESH_ENABLE_SECOND_DERIVATIVES

  // test that mesh function works correctly with non-zero
  // Elem::p_level() values.
#ifdef LIBMESH_ENABLE_AMR
  void test_p_level()
  {
    LOG_UNIT_TEST;

    ReplicatedMesh mesh(*TestCommWorld);

    MeshTools::Generation::build_cube (mesh,
                                       5, 5, 5,
                                       0., 1.,
                                       0., 1.,
                                       0., 1.,
                                       HEX20);

    // Bump the p-level for all elements. We will create a system with
    // a FIRST, LAGRANGE variable and then use the additional p-level
    // to solve with quadratic elements. Note: set_p_level(1) actually
    // _increases_ the existing variable order by 1, it does not set
    // it to 1.
    for (auto & elem : mesh.active_element_ptr_range())
      elem->set_p_level(1);

    EquationSystems es(mesh);
    System & sys = es.add_system<System> ("SimpleSystem");
    unsigned int u_var = sys.add_variable("u", FIRST, LAGRANGE);

    es.init();
    sys.project_solution(projection_function, nullptr, es.parameters);


    std::unique_ptr<NumericVector<Number>> mesh_function_vector
      = NumericVector<Number>::build(sys.comm());
    mesh_function_vector->init(sys.n_dofs(), sys.n_local_dofs(),
                               sys.get_dof_map().get_send_list(), false,
                               GHOSTED);

    sys.solution->localize(*mesh_function_vector,
                           sys.get_dof_map().get_send_list());


    // So the MeshFunction knows which variables to compute values for.
    // std::make_unique doesn't like if we try to use {u_var} in-place?
    std::vector<unsigned int> variables {u_var};

    auto mesh_function =
      std::make_unique<MeshFunction>(sys.get_equation_systems(),
                                        *mesh_function_vector,
                                        sys.get_dof_map(),
                                        variables);

    mesh_function->init();
    mesh_function->set_point_locator_tolerance(0.0001);
    DenseVector<Number> vec_values;
    std::string dummy;

    // Make sure the MeshFunction's values interpolate the projected solution
    // at the nodes
    for (const auto & node : mesh.local_node_ptr_range())
      {
        (*mesh_function)(*node, /*time=*/ 0., vec_values);
        Number mesh_function_value =
          projection_function(*node,
                              es.parameters,
                              dummy,
                              dummy);

        LIBMESH_ASSERT_NUMBERS_EQUAL
          (vec_values(0), mesh_function_value, TOLERANCE*TOLERANCE);
      }
  }
#endif // LIBMESH_ENABLE_AMR

#ifdef LIBMESH_HAVE_PETSC

  // A helper function that populates the solution data from output files
  DenseVector<Number> read_variable_info_from_output_data(const std::string & mesh_name,
                                                          const std::string & solutions_name)
  {
    DenseVector<Number> output;

    // Reading mesh and solution information from XDA files
    ReplicatedMesh mesh(*TestCommWorld);
    mesh.read(mesh_name);
    EquationSystems es(mesh);
    es.read(solutions_name, READ,
            EquationSystems::READ_HEADER |
                EquationSystems::READ_DATA |
                EquationSystems::READ_ADDITIONAL_DATA);
    es.update();

    // Pulling the correct system and variable information from
    // the XDA files (the default system name is "nl0")
    System & sys = es.get_system<System>("nl0");
    std::unique_ptr<NumericVector<Number>> mesh_function_vector =
        NumericVector<Number>::build(es.comm());
    mesh_function_vector->init(sys.n_dofs(), false, SERIAL);
    sys.solution->localize(*mesh_function_vector);

    // Pulling the total number of variables stored in XDA file
    std::vector<unsigned int> variables;
    sys.get_all_variable_numbers(variables);
    std::sort(variables.begin(),variables.end());

    // Setting up libMesh::MeshFunction
    MeshFunction mesh_function(es, *mesh_function_vector,
                                sys.get_dof_map(), variables);
    mesh_function.init();

    // Defining input parameters for MeshFunction::operator()
    const std::set<subdomain_id_type> * subdomain_ids = nullptr;
    const Point & p = Point(0.5, 0.5);

    // Supplying the variable values at center of mesh to output
    (mesh_function)(p, 0.0, output, subdomain_ids);

    return output;
  }

  // Tests the projection of Lagrange Vectors using MeshFunction
  void vectorMeshFunctionLagrange()
  {
    LOG_UNIT_TEST;

    // Populating the solution data using a helper function
    DenseVector<Number> output = read_variable_info_from_output_data("solutions/lagrange_vec_solution_mesh.xda","solutions/lagrange_vec_solution.xda");
    Gradient output_vec = VectorValue(output(0),output(1));

    // Expected value at center mesh
    Gradient output_expected = VectorValue(0.100977281077292,0.201954562154583);

    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(0)), libMesh::libmesh_real(output_expected(0)),
                            TOLERANCE * TOLERANCE);
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(1)), libMesh::libmesh_real(output_expected(1)),
                            TOLERANCE * TOLERANCE);
  }

  // Tests the projection of Nedelec Vectors using MeshFunction
  void vectorMeshFunctionNedelec()
  {
    LOG_UNIT_TEST;

    // Populating the solution data using a helper function
    DenseVector<Number> output = read_variable_info_from_output_data("solutions/nedelec_one_solution_mesh.xda","solutions/nedelec_one_solution.xda");
    Gradient output_vec = VectorValue(output(0),output(1));

    // Expected value at center mesh
    Gradient output_expected = VectorValue(0.0949202883998996,-0.0949202883918033);

    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(0)), libMesh::libmesh_real(output_expected(0)),
                            TOLERANCE * TOLERANCE);
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(1)), libMesh::libmesh_real(output_expected(1)),
                            TOLERANCE * TOLERANCE);
  }

  // Tests the projection of Raviart Thomas Vectors using MeshFunction
  void vectorMeshFunctionRaviartThomas()
  {
    LOG_UNIT_TEST;

    // Populating the solution data using a helper function
    DenseVector<Number> output = read_variable_info_from_output_data("solutions/raviart_thomas_solution_mesh.xda","solutions/raviart_thomas_solution.xda");
    Gradient output_vec = VectorValue(output(0),output(1));

    // Expected value at center mesh
    Gradient output_expected = VectorValue(0.0772539939808116,-0.0772537479511396);

    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(0)), libMesh::libmesh_real(output_expected(0)),
                            TOLERANCE * TOLERANCE);
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_vec(1)), libMesh::libmesh_real(output_expected(1)),
                            TOLERANCE * TOLERANCE);
  }

  // Tests MeshFunction for mixed scalar and vector variable outputs
  // In this case, the variables types are:
  //  - variable index: 0, variable family: Raviart Thomas, Order: First
  //  - variable index: 1, variable family: Monomial, Order: Constant
  //  - variable index: 2, variable family: Scalar, Order: First
  void mixedScalarAndVectorVariables()
  {
    LOG_UNIT_TEST;

    // Populating the solution data using a helper function
    DenseVector<Number> output = read_variable_info_from_output_data("solutions/raviart_thomas_solution_mesh.xda","solutions/raviart_thomas_solution.xda");
    Gradient output_raviart_thomas = VectorValue(output(0),output(1));

    // Expected value at center mesh
    Gradient raviart_thomas_expected = VectorValue(0.0772539939808116,-0.0772537479511396);
    Number monomial_expected = -0.44265566716952;
    Number scalar_expected = -2.86546e-18;

    // Checking Raviart Thomas variable family values
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_raviart_thomas(0)), libMesh::libmesh_real(raviart_thomas_expected(0)),
                            TOLERANCE * TOLERANCE);
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output_raviart_thomas(1)), libMesh::libmesh_real(raviart_thomas_expected(1)),
                            TOLERANCE * TOLERANCE);
    // Checking Monomial variable family value
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output(2)), libMesh::libmesh_real(monomial_expected),
                            TOLERANCE * TOLERANCE);
    // Checking scalar variable family value
    LIBMESH_ASSERT_FP_EQUAL(libMesh::libmesh_real(output(3)), libMesh::libmesh_real(scalar_expected),
                            TOLERANCE * TOLERANCE);
  }
#endif // LIBMESH_HAVE_PETSC
};

CPPUNIT_TEST_SUITE_REGISTRATION(MeshFunctionTest);
