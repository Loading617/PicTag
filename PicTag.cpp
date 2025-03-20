#include <gtkmm.h>
#include <exiv2/exiv2.hpp>
#include <iostream>
#include <vector>

class PicTag : public Gtk::Window {
public:
    PicTag();
    virtual ~PicTag() {}

private:
    void on_button_open();
    void on_button_save();
    void on_button_batch_open();
    void on_button_batch_save();
    void load_image(const std::string &filename);
    void load_metadata(const std::string &filename);
    void save_metadata(const std::string &filename);
    void add_to_history(const std::string &action, const std::string &filename);
    void undo_last_action();

    Gtk::Box m_VBox;
    Gtk::Button m_ButtonOpen, m_ButtonSave, m_ButtonBatchOpen, m_ButtonBatchSave, m_ButtonUndo;
    Gtk::Image m_Image;
    Gtk::FileChooserButton m_FileChooser;
    Gtk::FileChooserDialog m_BatchFileChooser;
    Gtk::Entry m_TagEntry;
    Gtk::Label m_TagLabel;

    std::string current_filename;
    std::vector<std::string> batch_filenames;
    std::vector<std::pair<std::string, std::string>> edit_history;
};

PicTag::PicTag()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_ButtonOpen("Open Image"),
      m_ButtonSave("Save Metadata"),
      m_ButtonBatchOpen("Batch Open"),
      m_ButtonBatchSave("Batch Save"),
      m_ButtonUndo("Undo"),
      m_FileChooser("Select an image", Gtk::FILE_CHOOSER_ACTION_OPEN),
      m_BatchFileChooser("Select multiple images", Gtk::FILE_CHOOSER_ACTION_OPEN) {

    set_title("PicTag");
    set_default_size(600, 400);

    m_BatchFileChooser.set_select_multiple(true);

    add(m_VBox);
    
    m_VBox.pack_start(m_ButtonOpen, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_ButtonBatchOpen, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_FileChooser, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Image, Gtk::PACK_EXPAND_WIDGET);
    m_VBox.pack_start(m_TagLabel, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_TagEntry, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_ButtonSave, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_ButtonBatchSave, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_ButtonUndo, Gtk::PACK_SHRINK);

    m_ButtonOpen.signal_clicked().connect(sigc::mem_fun(*this, &PicTag::on_button_open));
    m_ButtonSave.signal_clicked().connect(sigc::mem_fun(*this, &PicTag::on_button_save));
    m_ButtonBatchOpen.signal_clicked().connect(sigc::mem_fun(*this, &PicTag::on_button_batch_open));
    m_ButtonBatchSave.signal_clicked().connect(sigc::mem_fun(*this, &PicTag::on_button_batch_save));
    m_ButtonUndo.signal_clicked().connect(sigc::mem_fun(*this, &PicTag::undo_last_action));

    show_all_children();
}

void PicTag::on_button_open() {
    current_filename = m_FileChooser.get_filename();
    if (!current_filename.empty()) {
        load_image(current_filename);
        load_metadata(current_filename);
    }
}

void PicTag::on_button_save() {
    if (!current_filename.empty()) {
        add_to_history("Edit", current_filename);
        save_metadata(current_filename);
    }
}

void PicTag::on_button_batch_open() {
    if (m_BatchFileChooser.run() == Gtk::RESPONSE_OK) {
        batch_filenames = m_BatchFileChooser.get_filenames();
    }
}

void PicTag::on_button_batch_save() {
    for (const auto &filename : batch_filenames) {
        add_to_history("Batch Edit", filename);
        save_metadata(filename);
    }
}

void PicTag::load_image(const std::string &filename) {
    m_Image.set(filename);
}

void PicTag::load_metadata(const std::string &filename) {
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            m_TagEntry.set_text("No metadata found");
        } else {
            auto it = exifData.findKey(Exiv2::ExifKey("Exif.Image.Artist"));
            if (it != exifData.end()) {
                m_TagEntry.set_text(it->toString());
            }
        }
    } catch (Exiv2::Error &e) {
        std::cerr << "Error loading metadata: " << e.what() << std::endl;
    }
}

void PicTag::save_metadata(const std::string &filename) {
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
        image->readMetadata();
        image->exifData()["Exif.Image.Artist"] = m_TagEntry.get_text();
        image->writeMetadata();
    } catch (Exiv2::Error &e) {
        std::cerr << "Error saving metadata: " << e.what() << std::endl;
    }
}

void PicTag::add_to_history(const std::string &action, const std::string &filename) {
    edit_history.push_back({action, filename});
}

void PicTag::undo_last_action() {
    if (!edit_history.empty()) {
        auto last_action = edit_history.back();
        edit_history.pop_back();
        std::cout << "Undoing " << last_action.first << " on " << last_action.second << std::endl;
    }
}

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.Pic_Tag");
    PicTag editor;
    return app->run(editor);
}