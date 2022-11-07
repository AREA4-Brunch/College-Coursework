
class Random {

 public:

    Random(const unsigned long long seed) : seed(seed), prev(seed) {}

    void reset() { this->prev = this->seed; }

    /// Returns next unsigned int in given range (exclusive).
    /// Default range is max unsigned int.
    unsigned int nextInt(unsigned int max_int=~0U) {
        this->prev = this->a * this->prev + this->c;
        return this->prev % max_int;
    }

    /// Returns next unsigned long long in given range (exclusive).
    /// Default range is max unsigned long long.
    unsigned long long nextLongLong(unsigned long long max_int=~0ULL) {
        this->prev = this->a * this->prev + this->c;
        return this->prev % max_int;
    }


 private:

    const unsigned long long seed;
    unsigned long long prev;
    static constexpr unsigned long long a = 2862933555777941757ULL;
    static constexpr unsigned long long c = 3037000493ULL;

};
