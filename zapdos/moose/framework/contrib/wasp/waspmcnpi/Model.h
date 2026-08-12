#ifndef WASP_MCNP_MODEL_H
#define WASP_MCNP_MODEL_H

#include "waspcore/Interpreter.h"
#include "waspcore/decl.h"

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <ostream>
#include <unordered_map>
#include <utility>

namespace wasp
{
namespace mcnpi
{

class WASP_PUBLIC Model
{

public:

    void set_zaid_relative_atomic_mass_map(std::map<int, double>* map)
    {
        m_zaid_mass_map = map;
    }
    /**
     * @brief build this model given an MCNP document parse tree
     * 
     * @param document_root the root of the MCNP document parse tree
     * @param error the error stream to which model processing errors will be emitted
     * @return true iff no model processing errors occur, false otherwise
     */
    bool build(const NodeView& document_root, std::ostream& error);
    // Same thing, but without cell parsing
    bool build_limited(const NodeView& document_root, std::ostream& error);

    // Produce description of cell data
    void describe_cells(std::ostream& out) const;
    void describe_cell_csg(size_t csg_index, std::ostream& out) const;

    // Produce description of materials data
    void describe_materials_json(std::ostream& out) const;

    // CSG operations, ordered according to precendence
    enum class CSG_OP : std::uint8_t { 
        Parenthesis,
        Union,
        Intersection,
        Complement,
        Leaf, // surface by itself
        END_CSG_OP
    };

    CSG_OP csg_node_operator(size_t csg_node_index) const;

    enum class Surface_Sense : std::uint8_t
    {
        Inside, // less than zero according to surface equation
        Outside, // greater than zero according to surface equation
        Null // indicative of a non-terminal csg node
    };
    // Obtain the sense of the given node's left side
    // Note: Surface_Sense::Null indicates a subtree, not a surface
    // and subsequently csg_node's left references another csg node 
    Surface_Sense csg_left_sense(size_t csg_node_index) const;
    Surface_Sense csg_right_sense(size_t csg_node_index) const;

    // Obtain the left/right index for the given csg node
    // If the corresponding sense is Null this is a csg subtree index (m_csg_nodes)
    // else it is a surface index (m_surfaces)
    size_t csg_node_left(size_t csg_node_index) const;
    size_t csg_node_right(size_t csg_node_index) const;

    enum class Surface_Type : std::uint8_t
    {
        PLANE,// Ax+By+Cz-D=0, coeffs are A,B,C,D
        PX,   // x-D=0, coeff is D
        PY,   // y-D=0, coeff is D
        PZ,   // z-D=0, coeff is D
        // Sphere
        SPH,        // (x-X)^2+(y-Y)^2+(z-Z)^2-R^2=0, coeffs are X,Y,Z,R
        // Cylinder
        C_X,     // (y-Y)^2+(z-Z)^2-R^2=0, coeffs are Y,Z,R
        C_Y,     // (x-X)^2+(z-Z)^2-R^2=0, coeffs are X,Z,R
        C_Z,     // (y-Y)^2+(x-X)^2-R^2=0, coeffs are Y,X,R
        CX,      // Y^2+Z^2-R^2=0, coeff is R
        CY,      // X^2+Z^2-R^2=0, coeff is R
        CZ,      // Y^2+X^2-R^2=0, coeff is R
        // Cone
        K_X,     // sqrt((y-Y)^2+(z-Z)^2) -T(x-X)=0, coeffs X,Y,Z,T^2, +/-1
        K_Y,     // sqrt((x-X)^2+(z-Z)^2) -T(y-Y)=0, coeffs X,Y,Z,T^2, +/-1
        K_Z,     // sqrt((y-Y)^2+(x-X)^2) -T(z-Z)=0, coeffs X,Y,Z,T^2, +/-1
        KX,      // sqrt(Y^2+Z^2) -T(x-X)=0, coeffs X,T^2, +/-1
        KY,      // sqrt(X^2+Z^2) -T(y-Y)=0, coeffs Y,T^2, +/-1
        KZ,      // sqrt(X^2+Y^2) -T(z-Z)=0, coeffs Z,T^2, +/-1
        // Simple Quadric
        SQ,      // A(x-X)^2+B(y-Y)^2+C(z-Z)^2+2D(x-X)^2+2E(y-Y)^2+2F(z-Z)^2+G=0, coeffs A,B,C,D,E,F,G,X,Y,Z
        // General Quadric
        GQ,      // Ax^2+By^2+Cz^2+Dxy+Eyz+Fzx+Gx+Hy+Jz+K=0, coeffs A,B,C,D,E,F,G,H,J,K
        // Torus
        TX,     // (x-X)^2/B^2+(sqrt((y-Y)^2+(z-Z)^2)-A)^2/C^2-1=0, ceoffs X,Y,Z,A,B,C
        TY,     // (y-Y)^2/B^2+(sqrt((x-X)^2+(z-Z)^2)-A)^2/C^2-1=0, ceoffs X,Y,Z,A,B,C
        TZ,     // (z-Z)^2/B^2+(sqrt((x-X)^2+(y-Y)^2)-A)^2/C^2-1=0, ceoffs X,Y,Z,A,B,C
        // surfaces defined by points
        X,
        Y,
        Z,
        // Macro bodies
        BOX,   // arbitrarily oriented orthogonal box
        RPP,   // rectangular parallelepiped
        RCC,   // right circular cylinder
        HEX,   // right hexagonal prism
        REC,   // right elliptical cylinder 
        TRC,   // truncated right-angle cone
        ELL,   // ellipsoid
        WED,   // wedge
        ARB,   // arbitrary polyhedron
    };
    
    double surface_coefficient(size_t coeff_index) const;

    struct Transform{
        double o[3]={0,0,0};
        double r[3][3] = {{0,0,0},
                          {0,0,0},
                          {0,0,0}};
        // rotation in angles ?
        bool is_degrees = false;
    };
    Transform& transform(size_t trans_index) const;

    size_t surface_count() const {return m_surfaces.size();}
    size_t cell_count() const {return m_cells.size();}
    // Obtain the type of the surface 
    Surface_Type surface_type(size_t surface_index) const;
    // Performs a binary search for surface with given id
    // Returns surface_count() if none found
    size_t surface_index(int surface_id) const;

    // Obtain the index of the first surface coefficient (for use wih surface_coefficient)
    std::vector<double> surface_coefficients(size_t surface_index) const;
    
    // Performs a binary search for cell with given id
    // Returns cell_count() if none found
    size_t cell_index(int cell_id) const;
private:

    bool build_cells(const NodeView& document_root, std::ostream& error);
    bool build_materials(const NodeView& document_root, std::ostream& error);
    bool build_surfaces(const NodeView& document_root, std::ostream& error);
    bool build_transforms(const NodeView& document_root, std::ostream& error);

    // material build methods
    bool build_material(const NodeView& material_node, std::ostream& error);
    bool build_material_zaid(const NodeView& zaid_node, std::ostream& error);

    // cell build methods
    bool build_cell(const NodeView& cell_node, std::ostream& error);    
    
    // surface/macrobody build methods
    bool build_sphere(const NodeView& surface_node, std::ostream& error);
    bool build_simple_quadric(const NodeView& surface_node, std::ostream& error);
    bool build_general_quadric(const NodeView& surface_node, std::ostream& error);
    bool build_cylinder(const NodeView& surface_node, std::ostream& error);
    bool build_pcylinder(const NodeView& surface_node, std::ostream& error);
    bool build_cone(const NodeView& surface_node, std::ostream& error);
    bool build_xyzcone(const NodeView& surface_node, std::ostream& error);
    bool build_pcone(const NodeView& surface_node, std::ostream& error);
    bool build_torus(const NodeView& surface_node, std::ostream& error);
    bool build_plane(const NodeView& surface_node, std::ostream& error);
    bool build_abcplane(const NodeView& surface_node, std::ostream& error);
    bool build_points(const NodeView& surface_node, std::ostream& error);
    bool build_box(const NodeView& surface_node, std::ostream& error);
    bool build_hexagon(const NodeView& surface_node, std::ostream& error);
    bool build_rectangular_parallelepiped(const NodeView& surface_node, std::ostream& error);
    bool build_right_rectangular_cylinder(const NodeView& surface_node, std::ostream& error);
    bool build_right_eliptical_cylinder(const NodeView& surface_node, std::ostream& error);
    bool build_plane_points(const NodeView& surface_node, std::ostream& error);

    std::vector<Transform> m_transforms;

    // Geometry constructive solid geometry node that describes boolean operations 
    struct CSG_Node
    {
        // Left, Right indices into m_cell_csg_nodes (0 is reserved as null  node)
        size_t left = 0;
        size_t right = 0;
        bool is_null() const {return left == 0 && right == 0 && left_sense == Surface_Sense::Null && right_sense == Surface_Sense::Null && op == CSG_OP::END_CSG_OP;}
        // Do left,right reference a node (m_cell_csg_nodes) or a surface (m_surface)
        bool is_left_node() const {return left_sense == Surface_Sense::Null;}
        bool is_right_node() const {return right_sense == Surface_Sense::Null;}
        Surface_Sense left_sense = Surface_Sense::Null;
        Surface_Sense right_sense = Surface_Sense::Null;
        CSG_OP op = CSG_OP::END_CSG_OP;
    };

    // All model csg nodes, referenced by Cell::csg_index
    std::vector<CSG_Node> m_cell_csg_nodes;

    // All surface coefficients, referenced by Surface::coeff_start, count
    std::vector<double> m_surface_coeffs;

    /**
     * @brief Struct capturing the definition of a surface
     * Surfaces are 
     */
    struct Surface{        
        // The identifier of the surface
        size_t id=0;
        Surface_Type type;
        // Index into m_surface_coeffs
        size_t coeff_start=0;
        // Number of coefficients in m_surface_coeffs that belong to his Surface
        size_t coeff_count=0;
        // Line on which the surface is defined
        size_t line=0;
        size_t transform_id=0;
        // surface
        size_t periodic_surface_id=0;
    };
    std::vector<Surface> m_surfaces;

    struct Cell{
        size_t  id = 0;
        // material identifier - search m_materials for definitin
        size_t  mat_id = 0;
        // density ( rho > 0 = atomic density g/b-cm, < 0 mass density (g/cc) )
        double  rho = 0;
        // 0 - none, 1 - hexahedra (cuboidal), 2 - hexagonal
        int lattice_type = 0; 
        // index into m_cell_csg_nodes
        size_t csg_index = 0;
        // identifier of the universe to which this cell belongs
        size_t universe_id = 0; 
        // index into transformation data
        size_t trans_index = 0;
        // index into lattice fill data
        size_t fill_index = 0;
        
        // material nuclide atomic densities (atomic density g/b-cm)
        // ordered by the zaid specified by material referenced by mat_id
        std::vector<int> nuclide_zaids;
        std::vector<double> nuclide_densities;
        // TODO add additional cell parameter connection
    };
    std::vector<Cell> m_cells;
    
    struct Material{
        std::size_t id = 0;
        // index into the m_material_zaids
        std::size_t zaid_index = 0;
        std::size_t zaid_count = 0;
        // location of the first preceding comment declaration
        std::size_t column = 0;
        std::size_t line = 0;
        std::vector<std::string> metadata;
    };
    std::vector<Material> m_materials;
    // Map of material_id to index into m_materials
    std::map<size_t, size_t> m_material_id_index;
    struct Material_Zaid_Entry
    {
        size_t zaid=0;
        double value=0;
        char abx[4] = "";
    };
    std::vector<Material_Zaid_Entry> m_material_zaids;

    // Map of zaid (SSSZZAAA - S = State, Z = Atomic number, A = Atomic mass number id) 
    // to relative atomic mass (g/mol numerically equivalent to unified atomic mass units)
    std::map<int, double>* m_zaid_mass_map = nullptr;


    // csg tree recursive method 
    // Takes postfix ordering of csg expression (null node represent Intersection)
    // Returns index of new csg node, m_cell_csg_node.size() if error occurred
    size_t build_csg_tree(NodeView::Collection& postfix, std::ostream& error);
    // Converts csg geometry infix expression to postfix for effecient follow on processing
    bool convert_infix_postfix(const NodeView& cell_geom_node, NodeView::Collection& postfix, std::ostream& error);

    // process the given cell parameters
    bool build_cell_params(Cell& c, const NodeView& cell_params_node, std::ostream& error);

    // process the given cell mixture (requires mat_id and rho and material data to be processed)
    bool build_cell_material(Cell& c, const NodeView& cell_mixture_node, std::ostream& error);

    // obtain the surface or cell index given a cell_geom_exp_node... used during construction
    // returns true, iff the surface_index or csg_index has been found.
    // 
    bool get_index(const NodeView& cell_geom_exp_node, size_t& surface_index, size_t& csg_index, std::ostream& error) const;
    // linear search in reverse 
    size_t find_cell_index(int cell_id) const;

    bool build_cell_geom(Cell& c, const NodeView& cell_geom_node, std::ostream& error);
    bool build_cell_like(Cell& c, const NodeView& cell_like_node, std::ostream& error);

}; // end of Model class
} // end of namespace mcnpi
} // end of namespace wasp
#endif
