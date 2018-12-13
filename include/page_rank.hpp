
#ifndef PAGE_RANKING_HPP
#define PAGE_RANKING_HPP

#include "rank_interface.hpp"
#include "utils.hpp"

namespace singularity {
    class page_rank: public rank_interface
    {
    public:
        page_rank(double_type outlink_weight, uint16_t num_threads)
            :outlink_weight(outlink_weight), 
            num_threads(num_threads) 
            {};
        const uint32_t MAX_ITERATIONS = 1000;

        virtual std::shared_ptr<vector_t> process(
            const matrix_t& outlink_matrix,
            const vector_t& initial_vector,
            const vector_t& weight_vector,
            const additional_matrices_vector& additional_matrices
        );
    private:
        double_type outlink_weight;
        uint16_t num_threads;
        double_type const precision = 0.01;
        virtual std::shared_ptr<vector_t> calculate_rank(
            const matrix_t& outlink_matrix, 
            const additional_matrices_vector& additional_matrices,
            const vector_t& initial_vector,
            const vector_t& weight_vector
        );
        virtual std::shared_ptr<vector_t> iterate(
            const matrix_t& outlink_matrix, 
            const additional_matrices_vector& additional_matrices,
            const vector_t& previous,
            const vector_t& teleportation
        );
    };
}

#endif /* PAGE_RANKING_HPP */

