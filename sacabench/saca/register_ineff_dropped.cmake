# Syntax: SACA_REGISTER(<path to header> <c++ type>)

# Reference SACAS:
SACA_REGISTER("saca/external/deep_shallow.hpp"
    sacabench::reference_sacas::deep_shallow)

SACA_REGISTER("saca/external/divsufsort/divsufsort.hpp"
    sacabench::reference_sacas::div_suf_sort)

SACA_REGISTER("saca/external/divsufsort/divsufsort_par.hpp"
    sacabench::reference_sacas::div_suf_sort_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/external/msufsort/msufsort.hpp"
#    sacabench::reference_sacas::msufsort)

SACA_REGISTER("saca/external/saca_k/saca_k.hpp"
    sacabench::reference_sacas::saca_k)

SACA_REGISTER("saca/external/sads/sads.hpp"
    sacabench::reference_sacas::sads)

SACA_REGISTER("saca/external/sais/sais.hpp"
    sacabench::reference_sacas::sais)

SACA_REGISTER("saca/external/sais_lite/sais_lite.hpp"
    sacabench::reference_sacas::sais_lite)

SACA_REGISTER("saca/external/gsaca.hpp"
    sacabench::reference_sacas::gsaca)

SACA_REGISTER("saca/external/libsais.hpp"
    sacabench::reference_sacas::libsais_seq)

SACA_REGISTER("saca/external/libsais.hpp"
    sacabench::reference_sacas::libsais_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/external/qsufsort/qsufsort_wrapper.hpp"
#    sacabench::qsufsort_ext::qsufsort_ext)

# removed for energy INefficiency
#SACA_REGISTER("saca/external/dc3/dc3.hpp"
#        sacabench::reference_sacas::dc3)

# Our implementations:
  
# removed for energy INefficiency
#SACA_REGISTER("saca/deep_shallow/saca.hpp"
#    sacabench::deep_shallow::serial)

# DIDN'T WORK
#SACA_REGISTER("saca/deep_shallow/saca.hpp"
#   sacabench::deep_shallow::serial_big_buckets)

# removed for energy INefficiency
#SACA_REGISTER("saca/deep_shallow/saca.hpp"
#    sacabench::deep_shallow::parallel)

SACA_REGISTER("saca/bucket_pointer_refinement.hpp"
    sacabench::bucket_pointer_refinement::bucket_pointer_refinement)

SACA_REGISTER("saca/bucket_pointer_refinement_parallel.hpp"
    sacabench::bucket_pointer_refinement_parallel::bucket_pointer_refinement_parallel)


# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort.hpp"
#    sacabench::m_suf_sort::m_suf_sort2)

# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort_scan.hpp"
#    sacabench::m_suf_sort_scan::m_suf_sort_scan2)

# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort_v2.hpp"
#    sacabench::m_suf_sort::m_suf_sort3)

# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort_par.hpp"
#    sacabench::m_suf_sort::m_suf_sort_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort_scan_par.hpp"
#    sacabench::m_suf_sort_scan::m_suf_sort_scan_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/m_suf_sort_v2_par.hpp"
#    sacabench::m_suf_sort::m_suf_sort_v2_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/prefix_doubling.hpp"
#    sacabench::prefix_doubling::prefix_doubling)

# removed for energy INefficiency
#SACA_REGISTER("saca/prefix_doubling.hpp"
#    sacabench::prefix_doubling::prefix_discarding_2)

# removed for energy INefficiency
#SACA_REGISTER("saca/prefix_doubling.hpp"
#    sacabench::prefix_doubling::prefix_discarding_4)

# removed for energy INefficiency
#SACA_REGISTER("saca/prefix_doubling.hpp"
#    sacabench::prefix_doubling::prefix_discarding_4_parallel)

# removed for energy INefficiency
#SACA_REGISTER("saca/prefix_doubling.hpp"
#    sacabench::prefix_doubling::prefix_discarding_2_parallel)

SACA_REGISTER("saca/sais.hpp"
    sacabench::sais::sais)

SACA_REGISTER("saca/sais_with_parallel_induce.hpp"
    sacabench::sais_wip::sais_wip)

SACA_REGISTER("saca/parallel_sais.hpp"
    sacabench::parallel_sais::parallel_sais)

SACA_REGISTER("saca/sads.hpp"
    sacabench::sads::sads)

SACA_REGISTER("saca/gsaca/gsaca.hpp"
    sacabench::gsaca::gsaca)

# removed for energy INefficiency
#SACA_REGISTER("saca/gsaca/gsaca_new.hpp"
#        sacabench::gsaca::gsaca_new)

SACA_REGISTER("saca/gsaca/gsaca_parallel.hpp"
        sacabench::gsaca::gsaca_parallel)

# removed for energy INefficiency
#SACA_REGISTER("saca/dc7.hpp"
#    sacabench::dc7::dc7)

# removed for energy INefficiency
#SACA_REGISTER("saca/qsufsort.hpp"
#    sacabench::qsufsort::qsufsort)

# removed for energy INefficiency
#SACA_REGISTER("saca/naive.hpp"
#    sacabench::naive::naive)

# removed for energy INefficiency
#SACA_REGISTER("saca/naive.hpp"
#    sacabench::naive::naive_ips4o)

# removed for energy INefficiency
#SACA_REGISTER("saca/naive.hpp"
#    sacabench::naive::naive_ips4o_parallel)

# removed for energy INefficiency
#SACA_REGISTER("saca/naive.hpp"
#    sacabench::naive::naive_parallel)

# removed for energy INefficiency
#SACA_REGISTER("saca/sacak.hpp"
#    sacabench::sacak::sacak)

# removed for energy INefficiency
#SACA_REGISTER("saca/dc3.hpp"
#    sacabench::dc3::dc3)

# removed for energy INefficiency
#SACA_REGISTER("saca/div_suf_sort/saca.hpp"
#    sacabench::div_suf_sort::div_suf_sort)

# removed for energy INefficiency
#SACA_REGISTER("saca/nzSufSort.hpp"
#    sacabench::nzsufsort::nzsufsort)

# removed for energy INefficiency
#SACA_REGISTER("saca/dc3_lite.hpp"
#    sacabench::dc3_lite::dc3_lite)

# removed for energy INefficiency
#SACA_REGISTER("saca/dc3_par.hpp"
#    sacabench::dc3_par::dc3_par)

# removed for energy INefficiency
#SACA_REGISTER("saca/dc3_par2.hpp"
#    sacabench::dc3_par2::dc3_par2)

# removed for energy INefficiency
#SACA_REGISTER("saca/osipov/osipov_sequential.hpp"
#    sacabench::osipov::osipov_sequential)

# removed for energy INefficiency
#SACA_REGISTER("saca/osipov/osipov_sequential.hpp"
#    sacabench::osipov::osipov_sequential_wp)

# removed for energy INefficiency
#SACA_REGISTER("saca/osipov/osipov_parallel.hpp"
#    sacabench::osipov::osipov_parallel)

# removed for energy INefficiency
#SACA_REGISTER("saca/osipov/osipov_parallel.hpp"
#    sacabench::osipov::osipov_parallel_wp)

## removed for energy INefficiency
#if(SACA_ENABLE_CUDA)
#SACA_REGISTER("saca/osipov/osipov_gpu.hpp"
#    sacabench::osipov::osipov_gpu)
#endif()

