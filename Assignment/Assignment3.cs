using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

public class CustomerService
{
    private readonly DatabaseContext db;

    public CustomerService(DatabaseContext database)
    {
        db = database;
    }

    // ---------- SEARCH OPERATIONS ----------

    public List<Customer> FindCustomersByCountry(string country)
    {
        return FindCustomers(customer => customer.Country.Contains(country));
    }

    public List<Customer> FindCustomersByCompanyName(string companyName)
    {
        return FindCustomers(customer => customer.CompanyName.Contains(companyName));
    }

    public List<Customer> FindCustomersByContactName(string contactName)
    {
        return FindCustomers(customer => customer.ContactName.Contains(contactName));
    }

    // Single responsibility: reusable search logic
    private List<Customer> FindCustomers(Func<Customer, bool> filter)
    {
        return db.Customers
                 .Where(filter)
                 .OrderBy(customer => customer.CustomerID)
                 .ToList();
    }

    // ---------- EXPORT OPERATION ----------

    public string ExportCustomersToCsv(List<Customer> customers)
    {
        StringBuilder csvBuilder = new StringBuilder();

        foreach (Customer customer in customers)
        {
            csvBuilder.AppendFormat(
                "{0},{1},{2},{3}",
                customer.CustomerID,
                customer.CompanyName,
                customer.ContactName,
                customer.Country
            );
            csvBuilder.AppendLine();
        }

        return csvBuilder.ToString();
    }
}


public class Customer
{
    public int CustomerID { get; set; }
    public string CompanyName { get; set; }
    public string ContactName { get; set; }
    public string Country { get; set; }
}

public class DatabaseContext
{
    public List<Customer> Customers { get; set; }
}
