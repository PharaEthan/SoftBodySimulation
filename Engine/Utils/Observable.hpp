#pragma once

#include <vector>
#include <algorithm>
#include <functional>

namespace Exodia {
    
    template<class... T>
    class Observable {

        public:

            Observable() = default;

        public:

            void Add(std::function<void(T...)> observer)
            {
                _Observers.push_back(observer);
            }

            void AddOnce(std::function<void(T...)> observer)
            {
                _ObserversOnce.push_back(observer);
            }

            void NotifyObservers(T... value)
            {
                for (auto& observer : _Observers)
                    observer(value...);
                for (auto& observer : _ObserversOnce)
                    observer(value...);

                _ObserversOnce.clear();
            }

        private:

            std::vector<std::function<void(T...)>> _Observers;
            std::vector<std::function<void(T...)>> _ObserversOnce;
    };
};
