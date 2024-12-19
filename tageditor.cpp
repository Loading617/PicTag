#include <wx/wx.h>
#include <wx/listctrl.h>
#include <exiv2/exiv2.hpp>

class TagEditorFrame : public wxFrame {
public:
    TagEditorFrame(const wxString& title);

private:
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);

    wxListCtrl* metadataList;
    wxStaticBitmap* imageDisplay;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(TagEditorFrame, wxFrame)
    EVT_MENU(wxID_OPEN, TagEditorFrame::OnOpen)
    EVT_MENU(wxID_SAVE, TagEditorFrame::OnSave)
wxEND_EVENT_TABLE()

class TagEditorApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(TagEditorApp);

bool TagEditorApp::OnInit() {
    TagEditorFrame* frame = new TagEditorFrame("Picture Tag Editor");
    frame->Show(true);
    return true;
}

TagEditorFrame::TagEditorFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O");
    fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S");
    fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X");
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Image Viewer
    imageDisplay = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(400, 400));
    mainSizer->Add(imageDisplay, 1, wxEXPAND | wxALL, 10);

    // Metadata Viewer
    metadataList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(400, 400), wxLC_REPORT);
    metadataList->InsertColumn(0, "Tag", wxLIST_FORMAT_LEFT, 150);
    metadataList->InsertColumn(1, "Value", wxLIST_FORMAT_LEFT, 250);
    mainSizer->Add(metadataList, 1, wxEXPAND | wxALL, 10);

    SetSizer(mainSizer);
}

void TagEditorFrame::OnOpen(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Open Image", "", "", "Image Files (*.jpg;*.png)|*.jpg;*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString filePath = openFileDialog.GetPath();

    // Load and display the image
    wxImage image;
    if (image.LoadFile(filePath)) {
        imageDisplay->SetBitmap(wxBitmap(image.Scale(400, 400)));
    }

    // Load metadata using Exiv2
    try {
        Exiv2::Image::AutoPtr img = Exiv2::ImageFactory::open(filePath.ToStdString());
        img->readMetadata();

        // Clear existing metadata list
        metadataList->DeleteAllItems();

        // EXIF metadata
        Exiv2::ExifData& exifData = img->exifData();
        for (auto& md : exifData) {
            long index = metadataList->InsertItem(metadataList->GetItemCount(), wxString(md.key()));
            metadataList->SetItem(index, 1, wxString(md.toString()));
        }
    } catch (const Exiv2::Error& e) {
        wxMessageBox("Error reading metadata: " + wxString(e.what()), "Error", wxICON_ERROR);
    }
}

void TagEditorFrame::OnSave(wxCommandEvent& event) {
    // Example: Save changes to metadata (implement editing logic)
    wxMessageBox("Save functionality not yet implemented.", "Info", wxICON_INFORMATION);
}

void UpdateMetadata(const std::string& filePath, const std::string& key, const std::string& value) {
    try {
        Exiv2::Image::AutoPtr img = Exiv2::ImageFactory::open(filePath);
        img->readMetadata();

        Exiv2::ExifData& exifData = img->exifData();
        exifData[key] = value;

        img->writeMetadata();
    } catch (const Exiv2::Error& e) {
        wxMessageBox("Error updating metadata: " + wxString(e.what()), "Error", wxICON_ERROR);
    }
}
