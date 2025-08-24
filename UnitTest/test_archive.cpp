#include <gtest/gtest.h>
#include <hyue/FileSystemArchiveFactory.h>

using namespace hyue;

TEST(FileSystemArchive, list)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNIT_TEST_DIR, true);

    archive->load();

    EXPECT_EQ(archive->find("*archive*").size(), 1);
    EXPECT_EQ(archive->find("*archive*", false, true).size(), 0);
    EXPECT_EQ(archive->find("*.cpp", false, false).size(), 3);

    archive->unload();

    factory.destroy_instance(archive);
}