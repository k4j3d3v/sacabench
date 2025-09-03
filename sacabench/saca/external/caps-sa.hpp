#include <Suffix_Array.hpp>
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
                const std::size_t subproblem_count(0);
    			const std::size_t max_context(0);
                constexpr char lookup[4] = {'A', 'C', 'T', 'G'};

				std::size_t n = text.size();                // .length();
				std::string tmp(text.begin(), text.end());
    			for (size_t i = 0; i < text.size(); i++) {
    				unsigned char c = text[i];
    				std::cout << "char[" << i << "] = " << int(c) << " ('" << c << "')" << std::endl;
				}
// 				say we ignore this normalization
//                parlay::blocked_for(0, text.size(), 65536,
//      				[&, n](size_t i, size_t start, size_t end) {
//        				(void)i;
//       				for (size_t j = start; j < std::min(end, n); ++j) {
//          				char c = text[j];
////          				text.[j] = lookup[((std::toupper(c) & 0x6) >> 1)];
//        			};
//    			});
//
//            	if(n <= std::numeric_limits<uint32_t>::max())
//            	{
//
//                    external_saca_with_writable_text_one_size_only<sa_index, uint32_t, unsigned char>
//                    (text, out_sa, text.size(), run_caps_sa<uint64_t>);
//            	}
//            	else
            	{

                    external_saca_with_writable_text_one_size_only<sa_index, uint64_t, unsigned char>
                    (text, out_sa, text.size(), run_caps_sa<uint64_t>);
            	}


    }

    private:

        // Define helper functions (templates)
template <typename IndexType>
static void run_caps_sa(unsigned char* text, IndexType* sa, size_t n) {
    // Construct the suffix array using CaPS_SA
    // The template IndexType allows uint32_t or uint64_t
    CaPS_SA::Suffix_Array<IndexType> suf_arr(
        reinterpret_cast<char*>(text), // CaPS_SA expects char*
        n,                             // length of the text
	2,
	0
    );

    suf_arr.construct();


}
    };
} // namespace sacabench::reference_sacas
