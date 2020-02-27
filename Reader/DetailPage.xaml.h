#pragma once

#include "DetailPage.g.h"

namespace Reader {
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class DetailPage sealed {
	public:
		DetailPage();

		/*static property Windows::UI::Xaml::DependencyProperty^ BookProperty {
			Windows::UI::Xaml::DependencyProperty^ get() { return s_bookProperty; }
		}*/

		/*property ::Reader::ViewModels::BookViewModel^ Book {
			::Reader::ViewModels::BookViewModel^ get() {
				return safe_cast<::Reader::ViewModels::BookViewModel^>(GetValue(s_bookProperty));
			}

			void set(::Reader::ViewModels::BookViewModel^ value) {
				SetValue(s_bookProperty, value);
			}
		}*/

		property Windows::Foundation::Collections::IIterable<::Reader::ViewModels::BookViewModel^>^ Items {
			Windows::Foundation::Collections::IIterable<::Reader::ViewModels::BookViewModel^>^ get();
		}

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	internal:
		static void RegisterDependencyProperties();

	private:
		//static Windows::UI::Xaml::DependencyProperty^ s_bookProperty;
		Platform::String^ currentTag;
		Platform::String^ Rename;
		::Reader::ViewModels::BookViewModel^ m_lastSelectedBook;
		Windows::Storage::StorageFile^ file;
		Windows::Data::Pdf::PdfDocument^ pdf;
		Windows::Foundation::EventRegistrationToken m_sizeChangedEventRegistrationToken;
		Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;

		property Windows::Data::Pdf::PdfDocument^ PDF {
			void set(Windows::Data::Pdf::PdfDocument^ PDF) {
				pdf = PDF;
			}

			Windows::Data::Pdf::PdfDocument^ get() {
				return pdf;
			}
		}

		void DetailPage_BackRequested(Platform::Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ args);
		
		void OnBackRequested();

		void ChangeTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DeleteBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RenameBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DisplayRenameBookDialog();
		void DisplayDeleteBookDialog();
		
		void BookRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
		void TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void OpenFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		void LoadFile();
	
	};
}