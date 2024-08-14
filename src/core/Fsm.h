#pragma once

#include "Exception.h"

#include <memory>

namespace util
{
    template<typename T>
    struct FsmState
    {
        virtual ~FsmState() = 0;
        virtual void Enter(std::shared_ptr<T> entity) = 0;
        virtual void Exit(std::shared_ptr<T> entity) = 0;
        virtual void Execute(std::shared_ptr<T> entity) = 0;
    };


    template<typename T>
    class Fsm
    {
    public:

        Fsm(std::shared_ptr<T> inOwner) : m_owner(inOwner) { }

        void SetCurrentState(std::shared_ptr<FsmState<T>> state) { m_currentState = state; }
        void SetPreviousState(std::shared_ptr<FsmState<T>> state) { m_previousState = state; }
        void SetGlobalState(std::shared_ptr<FsmState<T>> state) { m_globalState = state; }

        void Update() const
        {
            if (m_globalState)   m_globalState.Execute(m_owner);
            if (m_currentState)  m_currentState.Execute(m_owner);
        }

        void ChangeState(std::shared_ptr<FsmState<T>> newState)
        {
            if (!newState)
                throw exc::CoreException("Trying to change into an invalid state");

            m_previousState = m_currentState;

            m_currentState.Exit(m_owner);
            m_currentState = newState;
            m_currentState.Enter(m_owner);
        }

        void RevertToPreviousState()
        {
            ChangeState(m_previousState);
        }

        std::shared_ptr<FsmState<T>> GetCurrentState() const { return m_currentState; }
        std::shared_ptr<FsmState<T>> GetPreviousState() const { return m_previousState; }
        std::shared_ptr<FsmState<T>> GetGlobalState() const { return m_globalState; }

    private:

        std::shared_ptr<T>              m_owner = nullptr;
        std::shared_ptr<FsmState<T>>    m_currentState = nullptr,
                                        m_previousState = nullptr,
                                        m_globalState = nullptr;

    };
}