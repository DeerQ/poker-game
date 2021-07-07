#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>
enum class Result { Win,
    Loss,
    Tie };
enum class Suit { Spades,
    Hearts,
    Diamonds,
    Clubs };
enum class Value {
    C2,
    C3,
    C4,
    C5,
    C6,
    C7,
    C8,
    C9,
    Ten,
    Jack,
    Queen,
    King,
    Ace
};
enum class HandState {
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    Straight,
    Flush,
    FullHouse,
    FourOFAKind,
    StrFlush
};
class Card {
public:
    Suit suit;
    Value val;
    Card(const std::string& c)
    {
        char value = c.at(0);
        char suit = c.at(1);
        switch (suit) {
        case 'S':
            this->suit = Suit::Spades;
            break;
        case 'H':
            this->suit = Suit::Hearts;
            break;
        case 'D':
            this->suit = Suit::Diamonds;
            break;
        case 'C':
            this->suit = Suit::Clubs;
            break;
        default:
            throw std::runtime_error(
                "The suit of the card is not among the acceptable values!");
            break;
        }
        switch (value) {
        case '2':
            val = Value::C2;
            break;
        case '3':
            val = Value::C3;
            break;
        case '4':
            val = Value::C4;
            break;
        case '5':
            val = Value::C5;
            break;
        case '6':
            val = Value::C6;
            break;
        case '7':
            val = Value::C7;
            break;
        case '8':
            val = Value::C8;
            break;
        case '9':
            val = Value::C9;
            break;
        case 'T':
            val = Value::Ten;
            break;
        case 'J':
            val = Value::Jack;
            break;
        case 'Q':
            val = Value::Queen;
            break;
        case 'K':
            val = Value::King;
            break;
        case 'A':
            val = Value::Ace;
            break;
        default:
            throw std::runtime_error(
                "The value of the card is not among the acceptable values!");
            break;
        }
    }
};

struct PokerHand {
    std::vector<Card> cards;
    std::map<Value, uint8_t> val_count;
    PokerHand(const char* pokerhand)
    {
        std::string cards_str { pokerhand };
        std::stringstream ss(cards_str);
        std::string s;
        while (std::getline(ss, s, ' ')) {
            cards.emplace_back(s);
        }
        if (5 != cards.size()) {
            throw std::runtime_error("Input string of cards are not valid!");
        }

        sort_cards();
        kind_aggregation();
    }

    void sort_cards()
    {
        std::sort(cards.begin(), cards.end(),
            [](const Card& c1, const Card& c2) { return c1.val <= c2.val; });
    }

    void kind_aggregation()
    {
        for (auto& t : cards) {
            ++val_count[t.val];
        }
    }

    bool are_of_same_suit() const
    {
        for (unsigned int i = 0; i < cards.size() - 1; i++) {
            if (cards.at(i).suit != cards.at(i + 1).suit)
                return false;
        }
        return true;
    }

    bool is_sequenced() const
    {
        for (unsigned int i = 0; i < cards.size() - 1; i++) {
            if (static_cast<uint8_t>(cards.at(i + 1).val) != (static_cast<uint8_t>(cards.at(i).val) + 1)) {
                if (cards.at(i + 1).val == Value::Ace && cards.at(0).val == Value::C2)
                    continue;
                else
                    return false;
            }
        }
        return true;
    }

    std::tuple<HandState, Value, Value> is_of_a_kind() const
    {
        std::vector<std::vector<Value>> mem(5);
        for (auto& t : val_count) {
            mem.at(t.second).push_back(t.first);
        }
        bool four = (mem.at(4).size() > 0);
        bool three = (mem.at(3).size() > 0);
        bool two2 = (mem.at(2).size() > 1);
        bool one2 = (mem.at(2).size() > 0);
        // Four of a kind
        if (four) {
            auto t = mem.at(4).at(0);
            return std::make_tuple(HandState::FourOFAKind, t, t);
        }
        // Full house
        else if (three && one2) {
            auto t3 = mem.at(3).at(0);
            auto t2 = mem.at(2).at(0);
            return std::make_tuple(HandState::FullHouse, t3, t2);
        }
        // Three of a kind
        else if (three) {
            auto t3 = mem.at(3).at(0);
            return std::make_tuple(HandState::ThreeOfAKind, t3, t3);
        }
        // Two pair
        else if (two2) {
            auto t20 = mem.at(2).at(0);
            auto t21 = mem.at(2).at(1);
            return std::make_tuple(HandState::TwoPair, std::max(t20, t21), std::min(t20, t21));
        }
        //One pair
        else if (one2) {
            auto t2 = mem.at(2).at(0);
            return std::make_tuple(HandState::OnePair, t2, t2);
        }
        return std::make_tuple(HandState::HighCard, Value::C2, Value::C2);
    }

    Result is_high_card(const PokerHand& opponent) const
    {
        if (cards.size() != opponent.cards.size()) {
            throw std::runtime_error("Player and the opponent do not have the same number of cards!");
        }
        for (int i = cards.size() - 1; i >= 0; i--) {
            if (cards.at(i).val > opponent.cards.at(i).val)
                return Result::Win;
            else if (cards.at(i).val < opponent.cards.at(i).val)
                return Result::Loss;
        }
        return Result::Tie;
    }

    std::tuple<HandState, Value, Value> get_best_hand() const
    {
        bool same_suit = are_of_same_suit();
        bool sequenced = is_sequenced();
        if (same_suit && sequenced) {
            return std::make_tuple(HandState::StrFlush, Value::C2, Value::C2);
        }
        auto of_a_kind = is_of_a_kind();

        if (HandState::FourOFAKind == std::get<0>(of_a_kind) || HandState::FullHouse == std::get<0>(of_a_kind)) {
            return of_a_kind;
        } else if (same_suit) {
            return std::make_tuple(HandState::Flush, Value::C2, Value::C2);
        } else if (sequenced) {
            return std::make_tuple(HandState::Straight, Value::C2, Value::C2);
        } else if (HandState::ThreeOfAKind == std::get<0>(of_a_kind)
            || HandState::TwoPair == std::get<0>(of_a_kind)
            || HandState::OnePair == std::get<0>(of_a_kind)) {
            return of_a_kind;
        }
        return std::make_tuple(HandState::HighCard, Value::C2, Value::C2);
    }

    Result compare(const PokerHand& opponent) const
    {
        auto my_state = this->get_best_hand();
        auto opponent_state = opponent.get_best_hand();

        if (std::get<0>(my_state) > std::get<0>(opponent_state))
            return Result::Win;
        else if (std::get<0>(my_state) < std::get<0>(opponent_state))
            return Result::Loss;
        else {
            if (std::get<0>(my_state) == HandState::StrFlush && std::get<0>(opponent_state) == HandState::StrFlush) {
                return this->is_high_card(opponent);
            } else if (std::get<0>(my_state) == HandState::FourOFAKind && std::get<0>(opponent_state) == HandState::FourOFAKind) {
                if (std::get<1>(my_state) > std::get<1>(opponent_state))
                    return Result::Win;
                if (std::get<1>(my_state) == std::get<1>(opponent_state))
                    return Result::Tie;
                else
                    return Result::Loss;
            } else if (std::get<0>(my_state) == HandState::FullHouse && std::get<0>(opponent_state) == HandState::FullHouse) {
                if (std::get<1>(my_state) > std::get<1>(opponent_state))
                    return Result::Win;
                if (std::get<1>(my_state) == std::get<1>(opponent_state))
                    return Result::Tie;
                else
                    return Result::Loss;
            } else if (std::get<0>(my_state) == HandState::Flush && std::get<0>(opponent_state) == HandState::Flush) {
                return this->is_high_card(opponent);
            } else if (std::get<0>(my_state) == HandState::Straight && std::get<0>(opponent_state) == HandState::Straight) {
                return this->is_high_card(opponent);
            } else if (std::get<0>(my_state) == HandState::ThreeOfAKind && std::get<0>(opponent_state) == HandState::ThreeOfAKind) {
                {
                    if (std::get<1>(my_state) > std::get<1>(opponent_state))
                        return Result::Win;
                    if (std::get<1>(my_state) == std::get<1>(opponent_state))
                        return Result::Tie;
                    else
                        return Result::Loss;
                }
            } else if (std::get<0>(my_state) == HandState::TwoPair && std::get<0>(opponent_state) == HandState::TwoPair) {
                if (std::get<1>(my_state) > std::get<1>(opponent_state))
                    return Result::Win;
                if (std::get<1>(my_state) == std::get<1>(opponent_state)) {
                    if (std::get<2>(my_state) > std::get<2>(opponent_state))
                        return Result::Win;
                    else if (std::get<2>(my_state) == std::get<2>(opponent_state)) {
                        return is_high_card(opponent);
                    } else
                        return Result::Loss;
                } else
                    return Result::Loss;
            } else if (std::get<0>(my_state) == HandState::OnePair && std::get<0>(opponent_state) == HandState::OnePair) {
                if (std::get<1>(my_state) > std::get<1>(opponent_state))
                    return Result::Win;
                if (std::get<1>(my_state) == std::get<1>(opponent_state)) {
                    return is_high_card(opponent);
                } else
                    return Result::Loss;
            }
            return is_high_card(opponent);
        }
    }
};

Result compare(const PokerHand& player, const PokerHand& opponent)
{
    return player.compare(opponent);
}

void test(std::string str1, std::string str2)
{
    PokerHand hand(str1.c_str());
    PokerHand hand2(str2.c_str());
    auto res = compare(hand, hand2);
    if (res == Result::Win) {
        std::cerr << "Win" << std::endl;
    }
    if (res == Result::Loss) {
        std::cerr << "Loss" << std::endl;
    }
    if (res == Result::Tie) {
        std::cerr << "Tie" << std::endl;
    }
}

int main()
{
    std::vector<std::string> v = {
        "KS AS TS QS JS",
        "AS AD AC AH JD",
        "2S AH 2H AS AC",
        "2H 3H 5H 6H 7H",
        "3D 4C 5H 6H 2S",
        "AH AC 5H 6H AS",
        "2S 2H 4H 5S 4C",
        "AH AC 5H 6H 7S",
        "4S 5H 6H TS AC"
    };
    for (int i = 0; i < v.size(); i++) {
        for (int j = 0; j < v.size(); j++) {
            PokerHand hand(v.at(i).c_str());
            PokerHand hand2(v.at(j).c_str());
            auto res = compare(hand, hand2);
            if (i < j)
                if (res != Result::Win)
                    std::cerr << "1 " << (int)res << " | " << v.at(i) << " | " << v.at(j) << std::endl;
            if (i > j)
                if (res != Result::Loss)
                    std::cerr << "3 " << (int)res << " | " << v.at(i) << " | " << v.at(j) << std::endl;
            if (i == j)
                if (res != Result::Tie)
                    std::cerr << i << " " << j << " " << (int)res << " | " << v.at(i) << " | " << v.at(j) << std::endl;
        }
    }
    //    test("AS AD AC AH JD", "AS AD AC AH JD"); //, Result::Loss)); // "Highest straight flush wins"
    //    test("2H 3H 4H 5H 6H", "KS AS TS QS JS"); //, Result::Loss)); // "Highest straight flush wins"
    //    test("2H 3H 4H 5H 6H", "AS AD AC AH JD"); //, Result::Win )); // "Straight flush wins of 4 of a kind"
    //    test("AS AH 2H AD AC", "JS JD JC JH 3D"); //, Result::Win )); // "Highest 4 of a kind wins"
    //    test("2S AH 2H AS AC", "JS JD JC JH AD"); //, Result::Loss)); // "4 Of a kind wins of full house"
    //    test("2S AH 2H AS AC", "2H 3H 5H 6H 7H"); //, Result::Win )); // "Full house wins of flush"
    //    test("AS 3S 4S 8S 2S", "2H 3H 5H 6H 7H"); //, Result::Win )); // "Highest flush wins"
    //    test("2H 3H 5H 6H 7H", "2S 3H 4H 5S 6C"); //, Result::Win )); // "Flush wins of straight"
    //    test("2S 3H 4H 5S 6C", "3D 4C 5H 6H 2S"); //, Result::Tie )); // "Equal straight is tie"
    //    test("2S 3H 4H 5S 6C", "AH AC 5H 6H AS"); //, Result::Win )); // "Straight wins of three of a kind"
    //    test("2S 3H 4H 5S AC", "AH AC 5H 6H AS"); //, Result::Win )); // "Low-ace straight wins of three of a kind"
    //    test("2S 2H 4H 5S 4C", "AH AC 5H 6H AS"); //, Result::Loss)); // "3 Of a kind wins of two pair"
    //    test("2S 2H 4H 5S 4C", "AH AC 5H 6H 7S"); //, Result::Win )); // "2 Pair wins of pair"
    //    test("6S AD 7H 4S AS", "AH AC 5H 6H 7S"); //, Result::Loss)); // "Highest pair wins"
    //    test("2S AH 4H 5S KC", "AH AC 5H 6H 7S"); //, Result::Loss)); // "Pair wins of nothing"
    //    test("2S 3H 6H 7S 9C", "7H 3C TH 6H 9S"); //, Result::Loss)); // "Highest card loses"
    //    test("4S 5H 6H TS AC", "3S 5H 6H TS AC"); //, Result::Win )); // "Highest card wins"
    //    test("2S AH 4H 5S 6C", "AD 4C 5H 6H 2C"); //, Result::Tie )); // "Equal cards is tie"

    return 0;
}
