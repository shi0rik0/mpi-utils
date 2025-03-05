#include <mpi_utils.hpp>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    vector<vector<int>> data = {{1, 2}, {3, 4, 5, 6}, {7, 8, 9}, {}};
    vector<int> local_data;
    scatter(data, local_data);

    cout << "Rank " << rank << ": local data: ";
    for (const auto& d : local_data) {
        cout << d << " ";
    }
    cout << endl;

    for (auto& d : local_data) {
        d *= 2;
    }

    vector<vector<int>> data2;
    gather(data2, local_data);

    if (rank == 0) {
        cout << "Rank 0: gathered data: ";
        cout << "{";
        for (size_t i = 0; i < data2.size(); ++i) {
            cout << "{";
            for (size_t j = 0; j < data2[i].size(); ++j) {
                cout << data2[i][j];
                if (j < data2[i].size() - 1) {
                    cout << ", ";
                }
            }
            cout << "}";
            if (i < data2.size() - 1) {
                cout << ", ";
            }
        }
        cout << "}" << endl;
    }

    MPI_Finalize();
    return 0;
}
