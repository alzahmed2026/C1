// MicrokernelGameEngine_Messages_Assets.cpp
// Microkernel مع دعم الرسائل بين الوحدات وتحميل الصور

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <map>
#include <functional>
#include <fstream>

struct Event { std::string name; bool handled = false; };

// --------------------- IMessageBus ---------------------
class IMessageBus {
public:
    using Callback = std::function<void(const std::string& message)>;

    void Subscribe(const std::string& messageType, Callback cb) {
        m_Subscribers[messageType].push_back(cb);
    }

    void SendMessage(const std::string& messageType, const std::string& payload) {
        auto it = m_Subscribers.find(messageType);
        if(it != m_Subscribers.end()){
            for(auto& cb : it->second) cb(payload);
        }
    }

private:
    std::unordered_map<std::string, std::vector<Callback>> m_Subscribers;
};

// --------------------- IModule ---------------------
class IModule {
public:
    virtual ~IModule() = default;
    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnEvent(Event& e) = 0;
    virtual const std::string& GetName() const = 0;
    virtual void SetMessageBus(IMessageBus* bus){ m_Bus = bus; }
protected:
    IMessageBus* m_Bus = nullptr;
};

using ModulePtr = std::shared_ptr<IModule>;

// --------------------- Microkernel ---------------------
class Microkernel {
public:
    void RegisterModule(const ModulePtr& module) {
        module->SetMessageBus(&m_MessageBus);
        m_Modules[module->GetName()] = module;
        m_Order.push_back(module->GetName());
        module->OnAttach();
    }

    void RunMainLoop(int frames=5,int msPerFrame=300){
        using clock=std::chrono::high_resolution_clock;
        auto last=clock::now();
        for(int i=0;i<frames;++i){
            auto now=clock::now();
            float dt=std::chrono::duration<float>(now-last).count();
            last=now;
            for(const auto& name:m_Order){
                m_Modules[name]->OnUpdate(dt);
            }
            Event e{"Click"};
            for(auto it=m_Order.rbegin();it!=m_Order.rend();++it){
                m_Modules[*it]->OnEvent(e);
                if(e.handled) break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(msPerFrame));
        }
    }

    IMessageBus& GetMessageBus(){ return m_MessageBus; }

private:
    std::unordered_map<std::string, ModulePtr> m_Modules;
    std::vector<std::string> m_Order;
    IMessageBus m_MessageBus;
};

// --------------------- Modules ---------------------
class RenderingModule : public IModule{
    std::string m_Name="Rendering";
    std::string m_Image;
public:
    void OnAttach() override {
        std::cout << "[Rendering] Attached\n";
        LoadImage("background.txt");
        if(m_Bus) m_Bus->SendMessage("ModuleReady", m_Name);
    }
    void OnDetach() override{ std::cout << "[Rendering] Detached\n"; }
    void OnUpdate(float dt) override{
        std::cout << "[Rendering] Draw image: " << m_Image << " (dt="<<dt<<")\n";
    }
    void OnEvent(Event& e) override{}
    const std::string& GetName() const override{ return m_Name; }

    void LoadImage(const std::string& filename){
        std::ifstream file(filename);
        if(file){
            std::string line, content;
            while(std::getline(file,line)) content += line + "\n";
            m_Image = content;
        }else m_Image="[No Image Found]";
    }
};

class UIModule : public IModule{
    std::string m_Name="UI";
public:
    void OnAttach() override{
        std::cout << "[UI] Attached\n";
        if(m_Bus) m_Bus->Subscribe("ModuleReady", [this](const std::string& module){
            std::cout << "[UI] Received message: Module ready - " << module << std::endl;
        });
    }
    void OnDetach() override{ std::cout << "[UI] Detached\n"; }
    void OnUpdate(float dt) override{
        std::cout << "[UI] Render: 'New Game' (dt="<<dt<<")\n";
    }
    void OnEvent(Event& e) override{
        if(e.name=="Click"){
            std::cout << "[UI] Click handled\n";
            e.handled=true;
        }
    }
    const std::string& GetName() const override{ return m_Name; }
};

// --------------------- main ---------------------
int main(){
    Microkernel kernel;

    kernel.RegisterModule(std::make_shared<RenderingModule>());
    kernel.RegisterModule(std::make_shared<UIModule>());

    kernel.RunMainLoop(5,300);

    return 0;
}
