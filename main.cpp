#include <array>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>

template <size_t N>
struct Weight {
    std::array<double, N> weights;
    [[no_unique_address]] Weight<(N + 1) / 2> parent;

    Weight(double _default)
        : weights{[_default] {
              std::array<double, N> tmp_weights;
              tmp_weights.fill(_default);
              return tmp_weights;
          }()},
          parent{weights}
    {
    }

    template <size_t NChild>
    Weight(const std::array<double, NChild>& children)
        : weights{[&children] {
              std::array<double, N> tmp_weights;
              for (size_t i = 0; i < NChild - N; i++) {
                  tmp_weights[i] = children[i * 2] + children[i * 2 + 1];
              }
              tmp_weights[N - 1] = (NChild % 2 == 0 ? children[NChild - 2] : 0.0) + children[NChild - 1];
              return tmp_weights;
          }()},
          parent{weights}
    {
    }

    void addWeightAt(size_t loc, double delta)
    {
        weights[loc] += delta;
        parent.addWeightAt(loc / 2, delta);
    }

    template <class URBG>
    std::pair<size_t, double> randomSelect(URBG& random_engine)
    {
        const auto [parent_loc, parent_rand] = parent.randomSelect(random_engine);
        const auto loc = parent_loc * 2;
        const auto weight = weights[loc];

        if (parent_rand < weight) {
            return {loc, parent_rand};
        } else {
            return {loc + 1, parent_rand - weight};
        }
    }
};

template <>
struct Weight<0>;

template <>
struct Weight<1> {
    double weight;

    Weight(double _default) : weight{_default} {}

    template <size_t NChild>
    Weight(const std::array<double, NChild>& children) : weight{std::accumulate(children.cbegin(), children.cend(), 0.0)}
    {
    }

    void addWeightAt(size_t, double delta) { weight += delta; }

    template <class URBG>
    std::pair<size_t, double> randomSelect(URBG& random_engine)
    {
        return {0, std::uniform_real_distribution{0.0, weight}(random_engine)};
    }
};

template <size_t NumLocation>
struct LocationSelecter {
private:
    Weight<NumLocation> weight;

public:
    LocationSelecter(double _default) : weight{_default} {}

    void addWeightAt(size_t loc, double delta) { weight.addWeightAt(loc, delta); }

    template <class URBG>
    size_t randomSelect(URBG& random_engine)
    {
        return weight.randomSelect(random_engine).first;
    }
};

int main()
{
    constexpr size_t N = 13;                 // select location from {0, 1, ... 12}
    LocationSelecter<N> loc_selecter{10.0};  // default weight = 10

    loc_selecter.addWeightAt(5, -10);  // decrease the weight of location 5 by 10 -> to be 0

    std::array<size_t, N> count = {};
    std::mt19937 random_engine{334};  // (334 is just a seed, you can choose any integer here)
    for (int i = 0; i < 100000; i++) {
        count[loc_selecter.randomSelect(random_engine)]++;  // count randomly selected locations
    }

    // you will see "0" for "Loc 5", and almost the same positive numbers for the other locations
    for (size_t i = 0; i < N; i++) {
        std::cout << "Loc " << i << "\t:\t" << count[i] << std::endl;
    }

    return 0;
}
