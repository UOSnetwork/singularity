
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdlib>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/exception/all.hpp>
#include "mapped_matrix_resizable.hpp"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "vector_based_matrix.hpp"
#include <mutex>

namespace singularity {
    typedef uint64_t money_t;
    typedef boost::multiprecision::number< boost::multiprecision::cpp_dec_float<10> > double_type;
    typedef boost::numeric::ublas::mapped_matrix_resizable<double_type, boost::numeric::ublas::row_major, std::map<std::uint64_t, double_type> > matrix_t;
    typedef boost::numeric::ublas::vector<double_type> vector_t;
    typedef boost::numeric::ublas::mapped_vector<double_type> sparce_vector_t;
    typedef uint32_t index_t;
    typedef boost::numeric::ublas::matrix_range<matrix_t> matrix_range_t;
    typedef boost::numeric::ublas::vector_range<vector_t> vector_range_t;
    typedef boost::numeric::ublas::range range_t;
    typedef boost::numeric::ublas::mapped_matrix_resizable<uint8_t, boost::numeric::ublas::row_major> byte_matrix_t;
    typedef std::vector<std::shared_ptr<boost::numeric::ublas::vector_based_matrix<double_type> > > additional_matrices_vector;
    
    typedef std::map<std::string, uint32_t> account_id_map_t;
    typedef std::map<std::string, double_type> account_activity_index_map_t;
    
    struct account_t {
        money_t amount;
        int height;
    };
    
    struct contribution_t {
        double_type koefficient;
        double_type rate;
    };
    
    typedef std::map<std::string, contribution_t> contribution_map;
    
    struct activity_index_detalization_t {
        double_type normalization_koefficient;
        std::map<std::string, double_type> base_index; 
        std::map<std::string, contribution_map> activity_index_contribution;
    };
    
    struct parameters_t {
        uint64_t precision = 10000000;
        uint64_t account_amount_threshold = 10000;
        uint64_t transaction_amount_threshold = 100;
        double_type outlink_weight = 0.7;
        double_type interlevel_weight = 0.1;
        uint clustering_m = 4;
        double_type clustering_e = 0.3;
        uint decay_period = 86400; // 24h = 86400 1s-blocks
        double_type decay_koefficient = 0.9;
        unsigned int num_threads = 1;
        double_type token_usd_rate = 1;
        bool include_detailed_data = false;
        bool extended_logging = false;
        bool use_diagonal_elements = false;
        double_type stack_contribution = 0;
        double_type weight_contribution = 0;
        double_type rank_calculation_precision = 0.01;
        bool consider_priorities_on_column_normalization = true;
        bool subtract_stack_after_activity_index_is_calculated = true;
        bool subtract_priority_after_activity_index_is_calculated = false;
        bool use_soft_descretization_function = false;
        bool debug_mode = false;
    };
    
    namespace normalization_tools
    {
        account_activity_index_map_t scale_activity_index_to_node_count(const account_activity_index_map_t& index_map); 
        account_activity_index_map_t scale_activity_index_to_1(const account_activity_index_map_t& index_map); 
        account_activity_index_map_t scale_activity_index(const account_activity_index_map_t& index_map, double_type new_norm);
    }
    
    namespace matrix_tools
    {
        void normalize_columns(matrix_t &m);
        void normalize_rows(matrix_t &m);
        sparce_vector_t calculate_correction_vector(const matrix_t& o);
        std::shared_ptr<matrix_t> resize(matrix_t& m, matrix_t::size_type size1, matrix_t::size_type size2);
        void prod( vector_t& out, const matrix_t& m, const vector_t& v, unsigned int num_threads);
        void prod( matrix_t& out, const matrix_t& in1, const matrix_t& in2);
        void partial_prod( vector_t& out, const matrix_t& m, const vector_t& v, range_t range);
        std::vector<range_t> split_range(range_t range, unsigned int max);
        vector_t discretize_hard(const vector_t& v);
        vector_t discretize_soft(const vector_t& v);
        double_type control_sum(const matrix_t& m);
        double_type control_sum(const boost::numeric::ublas::vector_based_matrix<double_type>& m);
    };
    
    class decay_manager_t
    {
    public:
        decay_manager_t(uint decay_period, double_type decay_koefficient):
            decay_period(decay_period), decay_koefficient(decay_koefficient) 
            {}
            double_type get_decay_value (uint64_t height)
            {
                uint64_t periods_passed = height / decay_period;
                
                double_type result = 1;
                
                while (periods_passed > 0) {
                    result *= decay_koefficient;
                    periods_passed--;
                }
                
                return result;
            }
    private:
        uint decay_period;
        double_type decay_koefficient;
        
    };
    
    class rate_t
    {
    public:
        rate_t(account_activity_index_map_t account_rate, account_activity_index_map_t content_rate)
            :account_rate(account_rate),content_rate(content_rate) {}
        account_activity_index_map_t get_account_rate() {
            return account_rate;
        }
        account_activity_index_map_t get_content_rate() {
            return content_rate;
        }
    private:
        account_activity_index_map_t account_rate;
        account_activity_index_map_t content_rate;
    };

    class id_registry
    {
    public:
        boost::optional<account_id_map_t::mapped_type> get_account_id(std::string name, bool allow_create);
        std::size_t get_account_count();
    private:
        std::mutex accounts_lock;
        account_id_map_t account_map;
    };
    
    class runtime_exception: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class validation_exception: public std::invalid_argument
    {
    public:
        using std::invalid_argument::invalid_argument;
    };
    
}

#endif /* UTILS_HPP */

