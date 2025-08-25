#include <gtest/gtest.h>

#include <hyue/FileSystemArchiveFactory.h>
#include <hyue/StringUtils.h>

using namespace hyue;

TEST(FileSystemArchive, open)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR, true);

    archive->load();

    EXPECT_NE(archive->open("CMakeLists.txt"), nullptr);
    EXPECT_EQ(archive->open("non_exists.txt"), nullptr);

    archive->unload();

    factory.destroy_instance(archive);
}

TEST(FileSystemArchive, create)
{
    {
        FileSystemArchiveFactory factory;
        auto archive = factory.create_instance(UNITTEST_DIR, true);

        archive->load();

        EXPECT_ANY_THROW(archive->create("test.txt"));

        archive->unload();

        factory.destroy_instance(archive);

    }

    {
        FileSystemArchiveFactory factory;
        auto archive = factory.create_instance(UNITTEST_DIR, false);

        archive->load();

        EXPECT_NE(archive->create("test.txt"), nullptr);

        archive->remove("test.txt");
        EXPECT_EQ(archive->open("test.txt"), nullptr);

        archive->unload();

        factory.destroy_instance(archive);
    
    }
}

TEST(FileSystemArchive, find)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR, true);

    archive->load();

    EXPECT_EQ(archive->find("*test_archive*").size(), 1);
    EXPECT_EQ(archive->find("*test_archive*", false, true).size(), 0);
    EXPECT_EQ(archive->find("*.cpp", false, false).size(), UNITTEST_COUNT);

    archive->unload();

    factory.destroy_instance(archive);
}


TEST(FileSystemArchive, list_file_info)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/", true);

    archive->load();

    auto items = archive->list_file_info(false, false);
    EXPECT_TRUE(items.size() > 0);

    auto it = std::find_if(items.begin(), items.end(), [](auto &item){
        return item.filename == UNITTEST_DIR "/CMakeLists.txt";
    });

    EXPECT_TRUE(it != items.end());

    EXPECT_EQ(it->dir, UNITTEST_DIR);
    EXPECT_EQ(it->basename, "CMakeLists.txt");
    EXPECT_FALSE(StringUtils::ends_with(it->dir, "/"));

    archive->unload();

    factory.destroy_instance(archive);
}

TEST(FileSystemArchive, exists)
{
    FileSystemArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/", true);

    archive->load();

    EXPECT_TRUE(archive->exists("CMakeLists.txt"));

    EXPECT_FALSE(archive->exists("non_exists.txt"));

    archive->unload();

    factory.destroy_instance(archive);
}