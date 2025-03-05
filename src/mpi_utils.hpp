#ifndef _MPI_UTILS_HPP_
#define _MPI_UTILS_HPP_

#include <mpi.h>
#include <vector>
#include <stdexcept>

template<typename>
inline constexpr bool always_false = false;


template<typename T>
void scatter(const std::vector<std::vector<T>>& data, std::vector<T>& local_data) {
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (rank == 0 && num_processes != int(data.size())) {
        throw std::runtime_error("Number of processes must be equal to the number of data elements");
    }

    std::vector<T> flattened_data;
    if (rank == 0) {
        for (const auto& d : data) {
            flattened_data.insert(flattened_data.end(), d.begin(), d.end());
        }
    }
    int recv_buf_size;
    std::vector<int> buf_sizes;
    if (rank == 0) {
        for (int i = 0; i < num_processes; i++) {
            buf_sizes.push_back(data[i].size());
        }
    }
    
    MPI_Scatter(buf_sizes.data(), 1, MPI_INT, &recv_buf_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local_data.resize(recv_buf_size);

    std::vector<int> sendcounts;
    std::vector<int> displs;
    if (rank == 0) {
        for (int i = 0; i < num_processes; i++) {
            sendcounts.push_back(data[i].size());
            displs.push_back(i == 0 ? 0 : displs[i - 1] + sendcounts[i - 1]);
        }
    }
    
    MPI_Datatype mpi_type;
    if constexpr (std::is_same_v<T, char>) {
        mpi_type = MPI_CHAR;
    } else if constexpr (std::is_same_v<T, int>) {
        mpi_type = MPI_INT;
    } else if constexpr (std::is_same_v<T, float>) {
        mpi_type = MPI_FLOAT;
    } else if constexpr (std::is_same_v<T, double>) {
        mpi_type = MPI_DOUBLE;
    } else {
        static_assert(always_false<T>, "Unsupported MPI datatype");
    }

    MPI_Scatterv(flattened_data.data(), sendcounts.data(), displs.data(), mpi_type,
        local_data.data(), recv_buf_size, mpi_type, 0, MPI_COMM_WORLD);
}

template<typename T>
void gather(std::vector<std::vector<T>>& data, const std::vector<T>& local_data) {
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    std::vector<int> local_sizes(num_processes);
    int local_size = local_data.size();
    MPI_Gather(&local_size, 1, MPI_INT, local_sizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<T> recv_buf;
    std::vector<int> displs;
    if (rank == 0) {
        int total_size = 0;
        for (auto& i : local_sizes) {
            total_size += i;
        }
        recv_buf.resize(total_size);
        displs.resize(num_processes);
        for (int i = 0; i < num_processes; i++) {
            displs[i] = i == 0 ? 0 : displs[i - 1] + local_sizes[i - 1];
        }
    }

    MPI_Datatype mpi_type;
    if constexpr (std::is_same_v<T, char>) {
        mpi_type = MPI_CHAR;
    } else if constexpr (std::is_same_v<T, int>) {
        mpi_type = MPI_INT;
    } else if constexpr (std::is_same_v<T, float>) {
        mpi_type = MPI_FLOAT;
    } else if constexpr (std::is_same_v<T, double>) {
        mpi_type = MPI_DOUBLE;
    } else {
        static_assert(always_false<T>, "Unsupported MPI datatype");
    }
    MPI_Gatherv(local_data.data(), local_size, mpi_type, recv_buf.data(),
        local_sizes.data(), displs.data(), mpi_type, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int h = 0;
        for (auto& i : local_sizes) {
            std::vector<T> tmp;
            for (int j = h; j < h + i; j++) {
                tmp.push_back(recv_buf[j]);
            }
            data.push_back(tmp);
            h += i;
        }
    }
}

#endif