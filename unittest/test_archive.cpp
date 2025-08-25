#include <gtest/gtest.h>

#include <hyue/FileSystemArchiveFactory.h>
#include <hyue/ZipArchiveFactory.h>

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

TEST(ZipArchiveFactory, non_exists)
{
    ZipArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/non_exists.zip");
    archive->load();

    EXPECT_EQ(archive->list().size(), 0);

    archive->unload();
    factory.destroy_instance(archive);
}


TEST(ZipArchiveFactory, list)
{
    ZipArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/test.zip");
    archive->load();

    EXPECT_EQ(archive->list().size(), 2);
    EXPECT_EQ(archive->list(false).size(), 0);
    EXPECT_EQ(archive->list(false, true).size(), 1);
    EXPECT_EQ(archive->list(true, true).size(), 3);

    archive->unload();
    factory.destroy_instance(archive);
}

TEST(ZipArchiveFactory, find)
{
    ZipArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/test.zip");
    archive->load();

    EXPECT_EQ(archive->find("*.jpg").size(), 1);
    EXPECT_EQ(archive->find("*.txt").size(), 1);
    EXPECT_EQ(archive->find("a/*.txt").size(), 0);
    EXPECT_EQ(archive->find("*a/*.txt").size(), 1);
    EXPECT_EQ(archive->find("*").size(), 2);

    archive->unload();
    factory.destroy_instance(archive);
}

TEST(ZipArchiveFactory, open)
{
    ZipArchiveFactory factory;
    auto archive = factory.create_instance(UNITTEST_DIR "/test.zip");
    archive->load();

    EXPECT_EQ(archive->open("test.txt"), nullptr);
    EXPECT_NE(archive->open("test/a/1.txt"), nullptr);

    auto data_stream = archive->open("test/a/1.txt");
    EXPECT_EQ(data_stream->get_as_string(), "hello, yue!\n");
    data_stream.reset();

    data_stream = archive->open("test/b/2.jpg");
    EXPECT_EQ(data_stream->get_as_string(), "");
    data_stream.reset();

    archive->unload();
    factory.destroy_instance(archive);
}