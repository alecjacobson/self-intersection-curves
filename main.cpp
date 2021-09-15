#include <igl/read_triangle_mesh.h>
#include <igl/writeOBJ.h>
#include <igl/doublearea.h>
#include <igl/copyleft/cgal/remesh_self_intersections.h>
#include <igl/slice_mask.h>
#include <igl/unique_edge_map.h>
#include <igl/remove_unreferenced.h>
#include <fstream>

int main(int argc, char * argv[])
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F0;
  igl::read_triangle_mesh(argv[1],V,F0);
  // Remove degenerate faces
  Eigen::MatrixXd A;
  igl::doublearea(V,F0,A);
  Eigen::MatrixXi F;
  igl::slice_mask(F0,(A.array()>1e-16).eval(),1,F);
  igl::copyleft::cgal::RemeshSelfIntersectionsParam params;
  params.detect_only = false;
  params.first_only = false;
  params.stitch_all = true;
  Eigen::MatrixXd W;
  Eigen::MatrixXi G;
  Eigen::MatrixXi IF;
  Eigen::VectorXi J,IM;
  igl::copyleft::cgal::remesh_self_intersections(V,F,params,W,G,IF,J,IM);
  Eigen::MatrixXi uE;
  Eigen::VectorXi EMAP,uEC,uEE;
  {
    Eigen::MatrixXi _;
    igl::unique_edge_map(G,_,uE,EMAP,uEC,uEE);
  }
  Eigen::MatrixXi E;
  igl::slice_mask(uE,(uEC.tail(uE.rows())-uEC.head(uE.rows())).array()>2,1,E);
  {
    Eigen::VectorXi _;
    igl::remove_unreferenced(Eigen::MatrixXd(W),Eigen::MatrixXi(E),W,E,_);
  }
  std::ofstream s(argv[2]);
  s<<
    W.format(Eigen::IOFormat(Eigen::FullPrecision,Eigen::DontAlignCols," ","\n","v ","","","\n"))<<
    (E.array()+1).format(Eigen::IOFormat(Eigen::FullPrecision,Eigen::DontAlignCols," ","\n","l ","","","\n"));
}
