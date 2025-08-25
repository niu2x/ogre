#include <gtest/gtest.h>
#include <hyue/FileSystemArchiveFactory.h>

using namespace hyue;

TEST(FileSystemArchive, list)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNIT_TEST_DIR, true);

    archive->load();

    EXPECT_EQ(archive->find("*test_archive*").size(), 1);
    EXPECT_EQ(archive->find("*test_archive*", false, true).size(), 0);
    EXPECT_EQ(archive->find("*.cpp", false, false).size(), UNIT_TEST_COUNT);

    archive->unload();

    factory.destroy_instance(archive);
}