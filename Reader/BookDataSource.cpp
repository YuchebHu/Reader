#include "pch.h"
#include "BookDataSource.h"
#include "BookViewModel.h"
#include <sqlite3.h>
#include <sstream>
#include <string>


using namespace Reader::Data;
using namespace Platform::Collections;
using namespace Windows::Foundation;


Vector<Book^>^ BookDataSource::s_books = ref new Vector<Book^>();
Vector<Platform::String^>^ BookDataSource::s_tags = ref new Vector<Platform::String^>();

BookDataSource::BookDataSource() {
	Database();
}


void BookDataSource::GetBooksFromDirectory(Platform::String^ path) {

}

Windows::Foundation::Collections::IVector<Platform::String^>^ Reader::Data::BookDataSource::GetAllTags() {
	return s_tags;
}

Windows::Foundation::Collections::IVector<Book^>^ BookDataSource::GetBookByTag(Platform::String^ Tag) {
	if (Tag == nullptr || Tag == L"All Books") {
		return GetAllBooks();
	}
	Platform::Collections::Vector<Book^>^ t_books = ref new Vector<Book^>();
	for (Book^ book : s_books) {
		if (book->Tag == Tag) {
			t_books->Append(book);
		}
	}
	return t_books;
}

void BookDataSource::Database() {
	GetBooksFromDatabase();
	GetTagsFromDatabase();
}

void Reader::Data::BookDataSource::GetBooksFromDatabase() {
	sqlite3* db = NULL;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc == SQLITE_OK) {
		printf("open");
	}
	else {
		printf("failed");
	}

	std::string sql1 = "CREATE TABLE tag(" \
		"tag			TEXT	NOT NULL);";
	std::string sql2 = "CREATE TABLE book("  \
		"name           TEXT    NOT NULL," \
		"tag			TEXT    NOT NULL REFERENCES tag(tag) ON DELETE CASCADE,"\
		"path			TEXT	NOT NULL UNIQUE);";

	if (SQLITE_OK == sqlite3_exec(db, sql2.c_str(), NULL, NULL, &ErrMsg)
		&& SQLITE_OK == sqlite3_exec(db, sql1.c_str(), NULL, NULL, &ErrMsg)) {
		std::string sql = "INSERT INTO TAG VALUES('All Books');";
		sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
	}
	else {
		std::string sql = "SELECT * FROM book";
		char** pResult;
		int nRow;
		int nCol;

		int rc = sqlite3_get_table(db, sql.c_str(), &pResult, &nRow, &nCol, &ErrMsg);
		int nIndex = nCol;

		for (int i = 0; i < nRow; ++i) {
			std::vector<Platform::String^> svec;
			for (int j = 0; j < nCol; ++j) {
				std::wstringstream wss;
				wss << pResult[nIndex++];
				svec.push_back(ref new Platform::String(wss.str().c_str()));
			}
			s_books->Append(ref new Book(svec[0], svec[1], svec[2]));
		}
		sqlite3_free_table(pResult);
	}
	sqlite3_close(db);
}

void Reader::Data::BookDataSource::GetTagsFromDatabase() {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);
	if (rc == SQLITE_OK) {
		std::string sql = "SELECT * FROM TAG;";
		char** pResult;
		int nRow;
		int nCol;
		rc = sqlite3_get_table(db, sql.c_str(), &pResult, &nRow, &nCol, &ErrMsg);
		if (rc == SQLITE_OK) {
			int nIndex = nCol;
			for (int i = 0; i < nRow; ++i) {
				for (int j = 0; j < nCol; ++j) {
					std::wstringstream wss;
					wss << pResult[nIndex++];
					s_tags->Append(ref new Platform::String(wss.str().c_str()));
				}
			}
			sqlite3_free_table(pResult);
		}
	}

	sqlite3_close(db);
}

std::string Reader::Data::BookDataSource::PStringToString(Platform::String^ pStr) {
	auto wchars = pStr->Data();
	std::wstring wstr(wchars);
	std::string str(wstr.begin(), wstr.end());
	return str;
}

void BookDataSource::UpdateBooks() {
	s_books->Clear();
	sqlite3* db = NULL;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);
	if (rc == SQLITE_OK) {
		std::string sql = "SELECT * FROM book";
		char** pResult;
		int nRow;
		int nCol;

		int rc = sqlite3_get_table(db, sql.c_str(), &pResult, &nRow, &nCol, &ErrMsg);
		int nIndex = nCol;

		for (int i = 0; i < nRow; ++i) {
			std::vector<std::wstring> svec;

			for (int j = 0; j < nCol; ++j) {
				std::wstringstream wss;
				wss << pResult[nIndex++];
				svec.push_back({ wss.str().c_str() });
			}

			s_books->Append(ref new Book(ref new Platform::String(svec[0].c_str()), ref new Platform::String(svec[1].c_str()), ref new Platform::String(svec[2].c_str())));
		}
		sqlite3_free_table(pResult);
	}
	sqlite3_close(db);
}

void Reader::Data::BookDataSource::DeleteBook(Book^ book) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);
	if (rc == SQLITE_OK) {
		std::string sql = "DELETE FROM BOOK WHERE NAME='" + PStringToString(book->Title) + "';";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			unsigned int i = 0;
			for (auto s_book : s_books) {
				if (s_book->Equals(book))
					break;
				++i;
			}
			s_books->RemoveAt(i);
		}

	}

	sqlite3_close(db);
}

void Reader::Data::BookDataSource::DeleteTag(Platform::String^ tag) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc = sqlite3_open(databasePath.c_str(), &db);
	if (rc == SQLITE_OK) {
		std::string sql = "DELETE FROM Tag WHERE tag='" + PStringToString(tag) + "';";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			for (auto s_book : s_books) {
				if (s_book->Tag == tag)
					s_book->Tag = L"All Books";
			}
			int index = 0;
			for (auto s_tag : s_tags) {
				if (s_tag == tag)
					break;
				++index;
			}
			s_tags->RemoveAt(index);
			sql = "UPDATE BOOK SET tag='All Books' WHERE tag='" + PStringToString(tag) + "';";
			rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);

		}
	}
	sqlite3_close(db);
}

void Reader::Data::BookDataSource::RenameBook(Book^ book, Platform::String^ Title) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc == SQLITE_OK) {
		std::string sql = "UPDATE BOOK SET NAME='" + PStringToString(Title) + "' WHERE NAME='" + PStringToString(book->Title) + "';";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			for (Book^ s_book : s_books) {
				if (s_book->Equals(book)) {
					s_book->Title = Title;
					break;
				}
			}
		}
	}

	sqlite3_close(db);

}

void Reader::Data::BookDataSource::RenameTag(Platform::String^ oldTag, Platform::String^ newTag) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;
	rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc == SQLITE_OK) {
		std::string sql = "UPDATE BOOK SET TAG='" + PStringToString(newTag) + "' WHERE TAG='" + PStringToString(oldTag) + "';";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			sql = "UPDATE TAG SET TAG='" + PStringToString(newTag) + "' WHERE TAG='" + PStringToString(oldTag) + "';";
			rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
			if (rc == SQLITE_OK) {
				for (Book^ s_book : s_books) {
					if (s_book->Tag == oldTag) {
						s_book->Tag = newTag;
					}
				}
				for (auto s_tag : s_tags) {
					if (s_tag == oldTag) {
						s_tag = newTag;
					}
				}
			}
			
		}
	}

	sqlite3_close(db);
}

void Reader::Data::BookDataSource::AddTag(Platform::String^ Tag) {
	sqlite3* db;
	char* ErrMsg;
	int rc;
	rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc == SQLITE_OK) {
		std::string sql = "INSERT INTO TAG VALUES('" + PStringToString(Tag) + "');";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			s_tags->Append(Tag);
		}
	}
}

void Reader::Data::BookDataSource::ChangeBookTag(Book^ book, Platform::String^ newTag) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);
	if (rc == SQLITE_OK) {
		std::string sql = "UPDATE BOOK SET tag='" + PStringToString(newTag) + "' WHERE path='" + PStringToString(book->Path) + "';";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (rc == SQLITE_OK) {
			for (auto s_book : s_books) {
				if (s_book->Equals(book))
					s_book->Tag = newTag;
			}
		}
	}
	sqlite3_close(db);
}

void BookDataSource::InsertBook(Book^ book) {
	sqlite3* db;
	char* ErrMsg = 0;
	int rc;

	rc = sqlite3_open(databasePath.c_str(), &db);

	if (rc == SQLITE_OK) {
		std::string sql = "INSERT INTO BOOK(name, tag, path) VALUES('" + PStringToString(book->Title) + "', '" + PStringToString(book->Tag) + "', '" + PStringToString(book->Path) + "');";
		rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &ErrMsg);
		if (SQLITE_OK == rc) {
			sqlite3_close(db);
			//UpdateBooks();
			s_books->Append(book);
		}
	}

	sqlite3_close(db);
}
