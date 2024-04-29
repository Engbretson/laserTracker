
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

int main() {
    // Given data
    Vector3d E1(4008.247, 862.714, 2863.860);
    Vector3d E12(2137.361, 944.795, -2355.180);
    Vector3d E3(1863.359, 908.285, 5221.612);

//    Vector3d E6(416.026, 1162.410, 7195.647);
    Vector3d E6(1034.918, 1161.631, -2372.503);

    Vector3d L1(-986.419947, 4462.744183, 824.472809);
    Vector3d L12(-4668.01465, 316.671811, 906.656876);
    Vector3d L3(2111.402938, 3712.446781, 869.932676);

    // Compute the rotation matrix
    Matrix3d L, E;
    L << L12 - L1, L3 - L1, (L12 - L1).cross(L3 - L1);
    E << E12 - E1, E3 - E1, (E12 - E1).cross(E3 - E1);
    Matrix3d R = L * E.inverse();

//    std::cout << "Rotation Matrix R:\n" << R << std::endl;

    // Compute translation vectors
    Vector3d TE = E1;
    Vector3d TL = L1;

    // Compute transformation matrix
    Matrix4d M;
    M.block<3, 3>(0, 0) = R;
    M.block<3, 1>(0, 3) = TL - R * TE;
    M.row(3) << 0, 0, 0, 1;

//    std::cout << "Rotation Matrix M:\n" << M << std::endl;

    // Apply transformation to E6
    Vector4d E6_homogeneous;
    E6_homogeneous << E6, 1;
    Vector4d L6_homogeneous = M * E6_homogeneous;
    Vector3d L6 = L6_homogeneous.head<3>();

    // Print transformed coordinates
    std::cout << "Transformed Coordinates L6: " << L6.transpose() << std::endl;

    return 0;
}
