﻿#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>

namespace xxr { namespace xcs_impl
{

    template <class Population>
    class MatchSet : public Population::ClassifierPtrSetType
    {
    public:
        using type = typename Population::type;
        using SymbolType = typename Population::SymbolType;
        using ConditionType = typename Population::ConditionType;
        using ActionType = typename Population::ActionType;
        using ConditionActionPairType = typename Population::ConditionActionPairType;
        using ConstantsType = typename Population::ConstantsType;
        using ClassifierType = typename Population::ClassifierType;
        using StoredClassifierType = typename Population::StoredClassifierType;
        using ClassifierPtrSetType = typename Population::ClassifierPtrSetType;
        using PopulationType = Population;
        using typename ClassifierPtrSetType::ClassifierPtr;

    protected:
        using Population::ClassifierPtrSetType::m_constants;
        using Population::ClassifierPtrSetType::m_availableActions;
        using Population::ClassifierPtrSetType::m_set;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<type> & situation, const std::unordered_set<ActionType> & unselectedActions, uint64_t timeStamp) const
        {
            auto cl = std::make_shared<StoredClassifierType>(situation, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
            cl->condition.setDontCareAtRandom(m_constants.dontCareProbability);

            return cl;
        }

    public:
        // Constructor
        using ClassifierPtrSetType::ClassifierPtrSetType;

        MatchSet(Population & population, const std::vector<type> & situation, uint64_t timeStamp, ConstantsType & constants, const std::unordered_set<ActionType> & availableActions)
            : ClassifierPtrSetType(constants, availableActions)
        {
            regenerate(population, situation, timeStamp);
        }

        // Destructor
        virtual ~MatchSet() = default;

        // GENERATE MATCH SET
        virtual void regenerate(Population & population, const std::vector<type> & situation, uint64_t timeStamp)
        {
            // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
            auto thetaMna = (m_constants.thetaMna == 0) ? m_availableActions.size() : m_constants.thetaMna;

            auto unselectedActions = m_availableActions;

            m_set.clear();

            while (m_set.empty())
            {
                for (auto && cl : population)
                {
                    if (cl->condition.matches(situation))
                    {
                        m_set.insert(cl);
                        unselectedActions.erase(cl->action);
                    }
                }

                // Generate classifiers covering the unselected actions
                if (m_availableActions.size() - unselectedActions.size() < thetaMna)
                {
                    auto coveringClassifier = generateCoveringClassifier(situation, unselectedActions, timeStamp);
                    if (!coveringClassifier->condition.matches(situation))
                    {
                        std::cerr <<
                            "Error: The covering classifier does not contain the current situation!\n"
                            "  - Current situation: ";
                        for (auto && s : situation)
                        {
                            std::cerr << s << " ";
                        }
                        std::cerr << "\n  - Covering classifier: " << *coveringClassifier << "\n" << std::endl;
                        assert(false);
                    }
                    population.insert(coveringClassifier);
                    population.deleteExtraClassifiers();
                    m_set.clear();
                }
            }
        }
    };

}}
