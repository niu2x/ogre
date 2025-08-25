#include <hyue/Root.h>
#include <hyue/FileSystemArchiveFactory.h>
#include <hyue/ZipArchiveFactory.h>
#include <hyue/math.h>

int main()
{


    hyue::ZipArchiveFactory factory;
    auto archive = factory.create_instance("./unittest/test.zip");
    archive->load();

    auto file_infos = archive->list_file_info(false, true);
    for (auto& item : file_infos) {
        printf("\n");
        printf("filename: %s\n", item.filename.c_str());
        printf("dir: %s\n", item.dir.c_str());
        printf("basename: %s\n", item.basename.c_str());
    }

    archive->unload();
    factory.destroy_instance(archive);

    return 0;
}