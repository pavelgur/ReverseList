#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <vector>

template<class T>
class TForwardList {
    struct TNode {
        TNode(T val)
            : Value(std::move(val))
        {}

        /**
         * returns the last element of the list
         * to continue pushing
         */
        std::shared_ptr<TNode> Push(T value) {
            assert(!Next);
            Next = std::make_shared<TNode>(std::move(value));
            return Next;
        }

        std::shared_ptr<TNode> Next;
        const T Value;
    };

    class TIterator : public std::iterator<std::forward_iterator_tag, TNode> {
    public:
        TIterator() = default;
        TIterator(std::shared_ptr<TNode> begin)
            : Cur(begin)
        {}

        bool operator!=(const TIterator& other) const noexcept {
            return Cur.get() != other.Cur.get();
        }

        auto& operator++() {
            assert(Cur);
            Cur = Cur->Next;
            return *this;
        }

        const auto& operator*() const {
            assert(Cur);
            return Cur->Value;
        }

    private:
        std::shared_ptr<TNode> Cur;
    };

public:
    using const_iterator = TIterator;

    auto begin() const noexcept {
        return TIterator(Root);
    }

    auto end() const noexcept {
        return TIterator();
    }

    void PushBack(T value) {
        if (!Root) {
            Last = Root = std::make_shared<TNode>(std::move(value));
            ++Length;
            return;
        }
        Last = Last->Push(std::move(value));
        ++Length;
    }

    void PopFront() {
        assert(Root);
        Root = Root->Next;
        --Length;
    }

    auto Size() const noexcept {
        return Length;
    }

    void Reverse() {
        Last = Root;

        if (!Root || !Root->Next) {
            return;
        }

        auto n = Root->Next;
        auto nn = n->Next;
        Root->Next.reset();

        for (;;) {
            n->Next = Root;
            Root = n;
            if (!nn) {
                break;
            }
            n = nn;
            nn = nn->Next;
        }
    }

private:
    std::shared_ptr<TNode> Root;
    std::shared_ptr<TNode> Last;
    size_t Length = 0;
};

int main() {
    srand(777);

    const auto size = 10000u;
    std::vector<int> vec;
    vec.reserve(size);

    for (auto i = 0u; i < vec.capacity(); ++i) {
        vec.push_back(rand());
    }

    TForwardList<typename decltype(vec)::value_type> list;
    for (const auto& v : vec) {
        list.PushBack(v);
    }
    assert(std::equal(vec.begin(), vec.end(), list.begin()));

    for (auto i = 0; i < 4; ++i) {
        list.Reverse();
        std::reverse(vec.begin(), vec.end());

        assert(vec.size() == list.Size());
        assert(std::equal(vec.begin(), vec.end(), list.begin()));
    }

    return EXIT_SUCCESS;
}
