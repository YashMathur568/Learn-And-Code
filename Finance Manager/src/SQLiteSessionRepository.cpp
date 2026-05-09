#include "SQLiteSessionRepository.h"

SQLiteSessionRepository::SQLiteSessionRepository(IDatabaseConnection& db) : db(db) {}

void SQLiteSessionRepository::save(const Session& session)                    {  }
Session SQLiteSessionRepository::findByToken(const std::string& token)        {  return {}; }
void SQLiteSessionRepository::deleteByToken(const std::string& token)         {  }
void SQLiteSessionRepository::deleteByUserId(int userId)                      {  }
