//
//  gsaca.hpp
//  gsaca
//
//  Created by David Piper on 09.05.18.
//  Copyright © 2018 David Piper. All rights reserved.
//

#pragma once

#include "external_saca.hpp"
#include <cstdint>
#include <iostream>
#include <util/span.hpp>
#include <util/string.hpp>
#include <util/alphabet.hpp>
#include <limits.h>
#include <stdlib.h>
#include <type_traits>

#include <gsaca-double-sort.hpp>
#include <gsaca-double-sort-par.hpp>
namespace sacabench::reference_sacas {
    using namespace sacabench;

    class dsh {
        public:
        static constexpr size_t EXTRA_SENTINELS = 0;
        static constexpr char const *NAME = "DSH";
        static constexpr char const *DESCRIPTION =
                "Lyndon words accelerate suffix sorting by Bertram et al.";
        static constexpr auto ds_algo = [](unsigned char* text, uint64_t* sa, std::size_t n) {
            ::gsaca_dsh<uint64_t, unsigned char>(text, sa, n);
        };
        template <typename sa_index>
        static void construct_sa(util::string_span text,
                                 const util::alphabet& alphabet,
                                 util::span<sa_index> out_sa) {
            run_ds((unsigned char*) text.data(), out_sa.data(), text.size(), ds_algo);
        }
        private:
        template <typename IndexType, typename AlgoFunc>
            static void run_ds(unsigned char* text, IndexType* sa, std::size_t n, AlgoFunc algo_func) {
                std::size_t size_with_sentinels = n + 2;
             
                unsigned char * const text_copy = (unsigned char*) malloc(size_with_sentinels * sizeof(unsigned char));
                for (std::size_t i = 1; i <= n; ++i) {
                    text_copy[i] = text[i-1];
                }
                text_copy[0] = 0; // Prepend sentinel
                text_copy[n+1] = 0; // Append sentinel
                uint64_t * temp_sa = (uint64_t *) malloc(size_with_sentinels * sizeof(uint64_t));
                // Call the algorithm function passed as parameter
                algo_func(text_copy, temp_sa, size_with_sentinels);

                // Copy results back, converting from uint64_t to IndexType
                size_t output_idx = 0;
                for (size_t i = 0; i < n + 2; ++i) {
                    if (temp_sa[i] != 0 && temp_sa[i] != n + 1) {  // Skip sentinel positions
                        sa[output_idx++] = static_cast<IndexType>(temp_sa[i] - 1);  // Adjust for removed leading sentinel
                    }
                }
                
                free(text_copy);
                free(temp_sa);
            }
    
    }; // class dsh
    class ds1 : public dsh {

    public:
        static constexpr size_t EXTRA_SENTINELS = 1;
        static constexpr char const *NAME = "DS1";
        static constexpr char const *DESCRIPTION =
                "Lyndon words accelerate suffix sorting by Bertram et al.";
        static constexpr auto ds_algo = [](unsigned char* text, uint64_t* sa, std::size_t n) {
            ::gsaca_ds1<uint64_t, unsigned char>(text, sa, n);
        };

    }; // class ds1

    class ds2 : public dsh {
    public:
        static constexpr size_t EXTRA_SENTINELS = 1;
        static constexpr char const *NAME = "DS2";
        static constexpr char const *DESCRIPTION = "GSACA DS2 algorithm";
        static constexpr auto ds_algo = [](unsigned char* text, uint64_t* sa, std::size_t n) {
            ::gsaca_ds2<uint64_t, unsigned char>(text, sa, n);
        };
    };

    class ds3 : public dsh {
    public:
        static constexpr size_t EXTRA_SENTINELS = 1;
        static constexpr char const *NAME = "DS3";
        static constexpr char const *DESCRIPTION = "GSACA DS3 algorithm";
        static constexpr auto ds_algo = [](unsigned char* text, uint64_t* sa, std::size_t n) {
            ::gsaca_ds3<uint64_t, unsigned char>(text, sa, n);
        };
    };

} // namespace sacabench::reference_sacas
