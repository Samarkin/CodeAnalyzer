#include "FolderProcessor.h"
#include <QDir>
#include <QDirIterator>

void FolderProcessor::process(const QString folderPath)
{
    emit startingProcessing(folderPath);
    FolderInfo info{new FolderInfoData};
    info->folderPath = folderPath;
    QDir folder{folderPath};
    QDirIterator it{folder, QDirIterator::Subdirectories};
    while (it.hasNext())
    {
        it.next();
        QFileInfo fileInfo = it.fileInfo();
        if (fileInfo.isFile())
        {
            FileInfo fi{fileInfo.filePath(), fileInfo.suffix()};
            TextFileInfo tfi{fi.path(), fi.ext()};
            BinaryFileInfo bfi{fi.path(), fi.ext()};
            // TODO: This should eventually be removed
            info->filesByExt[fi.ext()]++;

            // Read file
            QFile file{fi.path()};
            if (!file.open(QFile::ReadOnly))
            {
                info->inaccessibleFiles.append(fi);
                continue;
            }
            struct buffer {
                union {
                    char raw[4];
                    quint8 bytes[4];
                    quint32 char32;
                    quint16 char16;
                    quint8 char8;
                };
            } buffer;
            qint64 char_size;

            // BOM
            qint64 buffer_size = file.read(buffer.raw, sizeof(buffer));
            if (buffer_size >= 4 && buffer.char32 == 0xFFFE0000U) {
                // REVIEW: Is it safe to always assume Little Endian CPU?
                tfi.encoding = Encoding::Utf32BE;
                char_size = 4;
            }
            else if (buffer_size >= 4 && buffer.char32 == 0x0000FEFFU) {
                tfi.encoding = Encoding::Utf32LE;
                char_size = 4;
            }
            else if (buffer_size >= 2 && buffer.char16 == 0xFFFEU) {
                tfi.encoding = Encoding::Utf16BE;
                char_size = 2;
                file.seek(2);
            }
            else if (buffer_size >= 2 && buffer.char16 == 0xFEFFU) {
                tfi.encoding = Encoding::Utf16LE;
                char_size = 2;
                file.seek(2);
            }
            else if (buffer_size >= 3 && buffer.bytes[0] == 0xEF && buffer.bytes[1] == 0xBB && buffer.bytes[2] == 0xBF) {
                tfi.encoding = Encoding::Utf8;
                char_size = 1;
                file.seek(3);
            }
            else {
                tfi.encoding = Encoding::NoBom;
                char_size = 1;
                file.seek(0);
            }
            while (file.read(buffer.raw, char_size) > 0)
            {

            }
            info->textFiles.append(tfi);
        }

    }
    emit doneProcessing(info);
}
