#include <CtrlLib/CtrlLib.h>
#include <plugin/jpg/jpg.h>
#include <plugin/png/png.h>
#include <Draw/Draw.h>

using namespace Upp;

#define IMAGECLASS MyImages
#define IMAGEFILE <PicTag/images.iml>
#include <Draw/iml.h>

#include <exiv2/exiv2.hpp>

class PicTag : public TopWindow {
    ImageCtrl imageCtrl;
    EditString titleEdit, descEdit, authorEdit, dateEdit;
    String currentImagePath;

public:
    PicTag() {
        Title("PicTag");
        Sizeable().Zoomable();
        SetRect(0, 0, 600, 400);

        CtrlLayout(*this);

        Add(imageCtrl.LeftPosZ(10, 200).TopPosZ(10, 200));
        Add(titleEdit.LeftPosZ(220, 200).TopPosZ(10, 20).SetLabel("Title"));
        Add(descEdit.LeftPosZ(220, 200).TopPosZ(40, 20).SetLabel("Description"));
        Add(authorEdit.LeftPosZ(220, 200).TopPosZ(70, 20).SetLabel("Author"));
        Add(dateEdit.LeftPosZ(220, 200).TopPosZ(100, 20).SetLabel("Date"));

        Button& openBtn = *new Button();
        openBtn.SetLabel("Open Image");
        openBtn <<= [=] { OpenImage(); };
        Add(openBtn.LeftPosZ(220, 200).TopPosZ(130, 30));

        Button& saveBtn = *new Button();
        saveBtn.SetLabel("Save Metadata");
        saveBtn <<= [=] { SaveMetadata(); };
        Add(saveBtn.LeftPosZ(220, 200).TopPosZ(170, 30));
    }

    void OpenImage() {
        FileSel fs;
        fs.Type("Images", "*.jpg *.jpeg *.png");
        if (fs.ExecuteOpen()) {
            currentImagePath = fs.Get();
            imageCtrl.SetImage(StreamRaster::LoadAny(currentImagePath));
            LoadMetadata();
        }
    }

    void LoadMetadata() {
        try {
            Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(currentImagePath.ToStd());
            image->readMetadata();
            Exiv2::ExifData &exif = image->exifData();

            titleEdit <<= exif["Exif.Image.ImageDescription"].toString().c_str();
            descEdit  <<= exif["Exif.Photo.UserComment"].toString().c_str();
            authorEdit <<= exif["Exif.Image.Artist"].toString().c_str();
            dateEdit   <<= exif["Exif.Photo.DateTimeOriginal"].toString().c_str();
        } catch (const Exiv2::Error& e) {
            PromptOK(Format("Error reading metadata: %s", e.what()));
        }
    }

    void SaveMetadata() {
        try {
            Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(currentImagePath.ToStd());
            image->readMetadata();
            Exiv2::ExifData &exif = image->exifData();

            exif["Exif.Image.ImageDescription"] = titleEdit.Get().ToStd();
            exif["Exif.Photo.UserComment"] = descEdit.Get().ToStd();
            exif["Exif.Image.Artist"] = authorEdit.Get().ToStd();
            exif["Exif.Photo.DateTimeOriginal"] = dateEdit.Get().ToStd();

            image->setExifData(exif);
            image->writeMetadata();

            PromptOK("Metadata saved successfully.");
        } catch (const Exiv2::Error& e) {
            PromptOK(Format("Error saving metadata: %s", e.what()));
        }
    }
};

GUI_APP_MAIN {
    PicTag().Run();
}

