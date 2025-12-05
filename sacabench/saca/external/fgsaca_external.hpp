#include <sys/types.h>
#include <util/alphabet.hpp>
#include <tudocomp_stat/StatPhase.hpp>
#include "external_saca.hpp"
#include <cstddef>  // for std::size_t
#include <cstdint>  // for uint32_t, uint8_t
#include <vector>   // for std::vector
#include <algorithm> // for std::copy
#include <type_traits> // for std::is_same

// Include the actual FGSACA implementation
// The fgsaca library is linked via saca_bench_lib, so we can include directly
#include "fgsaca.hpp"  // This should work since lfgsaca dir is in include path


namespace sacabench::reference_sacas {
    using namespace sacabench::util;
    class fgsaca_external {
    public:
        static constexpr size_t EXTRA_SENTINELS = 1;
        static constexpr char const* NAME = "FGSACA";
        static constexpr char const* DESCRIPTION =
            "On the Optimisation of the GSACA Suffix Array Construction Algorithm by Jannik Olbrich, Enno Ohlebusch, Thomas Büchler";

        template <typename sa_index>
        static void construct_sa(util::string_span text,
                                 sacabench::util::alphabet alphabet,
                                 util::span<sa_index> out_sa) {
        run_fgsaca<sa_index>(text.data(), out_sa.data(), text.size());


    }

    private:

        // Define helper functions (templates)
    template <typename IndexType>
    static void run_fgsaca(const unsigned char* text, IndexType* sa, std::size_t n) {
        // Check if we can use the output array directly (C++17 compatible)
        if (std::is_same<IndexType, unsigned int>::value) {
            // Direct usage - no copying needed
            fgsaca<unsigned int, unsigned char>(text, reinterpret_cast<unsigned int*>(sa), n, 256);
        } else {
            // Need temporary array for type conversion
            std::vector<unsigned int> temp_sa(n);
            fgsaca<unsigned int, unsigned char>(text, temp_sa.data(), n, 256);
            // Copy with type conversion
            std::copy(temp_sa.begin(), temp_sa.end(), sa);
        }
            std::cerr<< "FGSACA: Suffix array construction completed for n = " << n << std::endl;
            for (size_t i = 0; i < n; ++i) {
                std::cerr << "sa[" << i << "] = " << sa[i] << std::endl;
            } 
    }

}; // class fgsaca
} // namespace sacabench::reference_sacas