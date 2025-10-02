#include <sys/types.h>
#include <util/alphabet.hpp>
#include <tudocomp_stat/StatPhase.hpp>
#include "external_saca.hpp"
#include <cstddef>  // for std::size_t
#include <cstdint>  // for uint32_t, uint8_t

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
        unsigned int *suff_a = new unsigned int[n];
        fgsaca<unsigned int, unsigned char>(text,suff_a, n, 256);
        // Copy results to output suffix array
        for (std::size_t i = 0; i < n; ++i) {
            sa[i] = static_cast<IndexType>(suff_a[i]);
        }
        delete[] suff_a;
    }

}; // class fgsaca
} // namespace sacabench::reference_sacas