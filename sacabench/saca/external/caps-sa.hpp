#include <Suffix_Array.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <util/alphabet.hpp>
#include <tudocomp_stat/StatPhase.hpp>
#include "external_saca.hpp"

namespace sacabench::reference_sacas {
    using namespace sacabench::util;
    class caps_sa {
    public:
        static constexpr size_t EXTRA_SENTINELS = 1;
        static constexpr char const* NAME = "CaPS-SA";
        static constexpr char const* DESCRIPTION =
            "Fast, parallel, and cache-friendly suffix array construction by Khan et al.";

        template <typename sa_index>
        static void construct_sa(util::string_span text,
                                 sacabench::util::alphabet alphabet,
                                 util::span<sa_index> out_sa) {

                    external_saca_with_writable_text_one_size_only<sa_index, uint64_t, unsigned char>
                    (text, out_sa, text.size(), run_caps_sa<uint64_t>);
    }

    private:

        // Define helper functions (templates)
template <typename IndexType>
static void run_caps_sa(unsigned char* text, IndexType* sa, size_t n) {
    bool ext_mem = false;
    bool output_lcp = false;
    std::size_t subproblem_count = 0;
    std::size_t max_context = 0;
    std::string ext_mem_path = "";
    
    // Use proper templating with the actual types passed
    CaPS_SA::Suffix_Array<unsigned char, IndexType> suf_arr(
        text, n, ext_mem, ext_mem_path, 
        subproblem_count, max_context, output_lcp);
        
    // Actually construct the suffix array
    suf_arr.construct();
    auto out_sa = suf_arr.SA();
    
    // Copy the result to the output suffix array
    for (size_t i = 0; i < n; ++i) {
        sa[i] = out_sa[i];
    }
}
    };
} // namespace sacabench::reference_sacas
