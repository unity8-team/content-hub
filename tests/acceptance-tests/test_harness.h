/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */
#ifndef TEST_HARNESS_H_
#define TEST_HARNESS_H_

#include <QObject>
#include <QTimer>

#include <functional>
#include <list>

namespace test
{
class TestHarness : public QObject
{
    Q_OBJECT

  public:
    TestHarness(QObject* parent = nullptr) : QObject(parent)
    {
    }
    
    virtual ~TestHarness()
    {
    }

    void add_test_case(const std::function<void()>& test_case)
    {
        known_test_cases.push_back(test_case);
    }

  private:
    std::list<std::function<void()>> known_test_cases;

  private Q_SLOTS:    
    void run() 
    {
        for(auto test_case : known_test_cases)
        {
            test_case();
        }
    }
};
}

#endif // TEST_HARNESS_H_
