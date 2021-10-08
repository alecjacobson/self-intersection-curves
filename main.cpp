#include <igl/read_triangle_mesh.h>
#include <igl/writeOBJ.h>
#include <igl/doublearea.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/slice_mask.h>
#include <igl/unique.h>
#include <igl/slice.h>
#include <igl/find.h>
#include <igl/unique_edge_map.h>
#include <igl/remove_unreferenced.h>
#include <fstream>
#include <iostream>

int main(int argc, char * argv[])
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F0;
  igl::read_triangle_mesh(argv[1],V,F0);
  // Remove degenerate faces
  Eigen::MatrixXd A;
  igl::doublearea(V,F0,A);
  Eigen::MatrixXi F;
  Eigen::VectorXi J;
  igl::find((A.array()>1e-16).eval(),J);
  igl::slice(F0,J,1,F);
  // Compute which faces intersect
  igl::copyleft::cgal::RemeshSelfIntersectionsParam params;
  params.detect_only = true;
  params.first_only = false;
  params.stitch_all = true;
  Eigen::VectorXi I;
  {
    Eigen::MatrixXd W;
    Eigen::MatrixXi G;
    Eigen::MatrixXi IF;
    Eigen::VectorXi K,IM;
    igl::copyleft::cgal::remesh_self_intersections(V,F,params,W,G,IF,K,IM);
    // Unique list of face ids
    Eigen::VectorXi U;
    igl::unique(IF,U);
    // Remap indices to mesh before removing degenerates
    igl::slice(J,U,1,I);
  }
  std::cout<<I.transpose()<<std::endl;
}
