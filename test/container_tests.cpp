/*******************************************************************************
 * test/container_tests.cpp
 *
 * Copyright (C) 2018 Marvin Löbel <loebel.marvin@gmail.com>
 *
 * All rights reserved. Published under the BSD-3 license in the LICENSE file.
 ******************************************************************************/

#include <gtest/gtest.h>
#include <util/span.hpp>
#include <util/container.hpp>

TEST(Container, construct_empty) {
    container<uint8_t> c;
}

TEST(Container, make_container) {
    container<uint8_t> c = make_container<uint8_t>(10);
    ASSERT_EQ(c.size(), 10);
}
