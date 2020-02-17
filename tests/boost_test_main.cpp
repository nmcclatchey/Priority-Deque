/// \file
/// \brief  Defines entrypoint for Boost.Test across multiple translation units.

//  Specify this as the base file used for Boost-based unit tests.
//  To trigger execution after build, use post-build step:
//    "$(TARGET_OUTPUT_DIR)$(TARGET_OUTPUT_BASENAME).exe" --result_code=no --report_level=no --random=1 --show_progress
#define BOOST_TEST_MODULE PriorityDeque
#include <boost/test/included/unit_test.hpp>

//  Note:
//    Boost.Test supports header-only with multiple translation units.
//  https://www.boost.org/doc/libs/1_71_0/libs/test/doc/html/boost_test/adv_scenarios/single_header_customizations/multiple_translation_units.html
//    To use this, each other translation unit must include the line
//  ```
//  #include <boost/test/unit_test.hpp>
//  ```
//  And must not (re-)define BOOST_TEST_MODULE
