#include "fl/db/Database.hpp"

namespace fl::db {
    
    std::unordered_map<std::string, Ref<Database>> Database::databases_;

    Database::Database(sql::mysql::MySQL_Driver* driver)
        : driver_(driver) {}
    Database::~Database() {}

    Ref<Database> Database::Innit(std::string_view db_name)
    {
        if (HasDatabase(db_name))
            return Get(db_name);

        Database* db = new Database(sql::mysql::get_driver_instance());
        databases_.insert(std::make_pair(db_name, db));

        return Get(db_name);
    }

    Ref<Database> Database::Get(std::string_view db_name) 
    {
        return databases_.at(db_name.data());
    }
    void Database::Remove(std::string_view db_name)
    {
        if (!HasDatabase(db_name))
            return;

        databases_.erase(db_name.data());
    }

    bool Database::HasDatabase(std::string_view db_name) 
    {
        return databases_.find(db_name.data()) != databases_.end();
    }

    bool Database::Connect(sql::ConnectOptionsMap options) 
    {
        try
        {
            connection_.reset(driver_->connect(options));
        }
        catch(std::exception const& e)
        {
            std::cerr << e.what() << '\n';
        }

        return IsConnected();
    }
    bool Database::Connect(std::string_view host, 
                std::string_view user, 
                std::string_view password) 
    {
        try
        {
            connection_.reset(driver_->connect(host.data(), user.data(), password.data()));
        }
        catch(std::exception const& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        return IsConnected();
    }

    void Database::SetActiveSchema(std::string_view scheme)
    {
        if (!IsConnected())
            return;

        connection_->setSchema(scheme.data());
    }

    Ref<sql::ResultSet> Database::Execute(std::string_view query)
    {
        Scope<sql::Statement> statement(connection_->createStatement());
        Ref<sql::ResultSet> result;

        try
        {
            Scope<sql::ResultSet> set(statement->executeQuery(query.data()));
            result = std::move(set);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        return result;
    }

    bool Database::IsConnected() const 
    {
        if (!connection_) 
            return false;

        return connection_->isValid(); 
    }

    void Database::Disconnect() const 
    {
        if (!IsConnected())
            return;

        connection_->close();
    }

    
} // namespace fl::db 
