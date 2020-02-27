#pragma once

namespace Reader {
	namespace Data {
		public ref class BookDataSource sealed {
		public:
			static Windows::Foundation::Collections::IVector<Book^>^ GetAllBooks() {
				return s_books;
			}

			static Windows::Foundation::Collections::IVector<Platform::String^>^ GetAllTags();
			
			static Windows::Foundation::Collections::IVector<Book^>^ GetBookByTag(Platform::String^);

			static Book^ GetBookByTitle(Platform::String^ title) {
				Book^ book = ref new Book();
				for (auto s_book : s_books) {
					if (s_book->Title == title) {
						book = s_book;
						break;
					}
				}
				return book;					   
			}

			static BookDataSource^ GetBookDataSource() {
				static BookDataSource^ s_instance = ref new BookDataSource();
				return s_instance;
			}

			void InsertBook(Book^ book);
			void DeleteBook(Book^ book);
			void DeleteTag(Platform::String^ tag);
			void RenameBook(Book^ book, Platform::String^ title);
			void RenameTag(Platform::String^ oldTag, Platform::String^ newTag);
			void AddTag(Platform::String^ Tag);
			void ChangeBookTag(Book^ book, Platform::String^ newTag);

		private:
			BookDataSource();
			void GetBooksFromDirectory(Platform::String^ path);
			static Platform::Collections::Vector<Book^>^ s_books;
			static Platform::Collections::Vector<Platform::String^>^ s_tags;
			std::string databasePath = "C:/Users/yucheb/AppData/Local/Packages/8b19f961-5b6e-4f3f-8694-f3e9a77e513b_r66vk35y1p316/LocalState/books.db";
			std::string PStringToString(Platform::String^);

			void Database();
			void GetBooksFromDatabase();
			void GetTagsFromDatabase();
			void UpdateBooks();
		};
	}
}