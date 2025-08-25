#include <hyue/Root.h>
#include <hyue/FileSystemArchiveFactory.h>
#include <hyue/math.h>

int main()
{
    hyue::FileSystemArchiveFactory factory;
    auto archive = factory.create_instance("./Docs");
    archive->load();

    auto file_infos = archive->list_file_info(true);
    for(auto &item: file_infos) {
        printf("\n"); 
        printf("filename: %s\n", item.filename.c_str());
        printf("dir: %s\n", item.dir.c_str());
        printf("basename: %s\n", item.basename.c_str());
    }

    archive->unload();
    factory.destroy_instance(archive);

    return 0;
}