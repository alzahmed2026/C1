#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

struct Event {
    std::string name;
    bool handled = false;
};

class Layer {
public:
    explicit Layer(const std::string& name = "Layer") : m_Name(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float deltaTime) {}
    virtual void OnEvent(Event& e) {}

    const std::string& GetName() const { return m_Name; }

private:
    std::string m_Name;
};

using LayerPtr = std::shared_ptr<Layer>;

class LayerStack {
public:
    ~LayerStack() { m_Layers.clear(); }

    void PushLayer(const LayerPtr& layer) {
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
        m_LayerInsertIndex++;
        layer->OnAttach();
    }

    void PushOverlay(const LayerPtr& overlay) {
        m_Layers.emplace_back(overlay);
        overlay->OnAttach();
    }

    std::vector<LayerPtr>& GetLayers() { return m_Layers; }

private:
    std::vector<LayerPtr> m_Layers;
    unsigned int m_LayerInsertIndex = 0;
};

class RenderingLayer : public Layer {
public:
    RenderingLayer() : Layer("Rendering") {}

    void OnAttach() override {
        std::cout << "[Rendering] Attached\n";
    }
    void OnDetach() override { std::cout << "[Rendering] Detached\n"; }
    void OnUpdate(float deltaTime) override {
        std::cout << "[Rendering] Draw frame (dt=" << deltaTime << ")\n";
    }
};

class UILayer : public Layer {
public:
    UILayer() : Layer("UI") {}

    void OnAttach() override { std::cout << "[UI] Attached\n"; }
    void OnDetach() override { std::cout << "[UI] Detached\n"; }
    void OnUpdate(float deltaTime) override {
        std::cout << "[UI] Render text: 'New Game' (dt=" << deltaTime << ")\n";
    }
    void OnEvent(Event& e) override {
        if (e.name == "Click") {
            std::cout << "[UI] Click handled\n";
            e.handled = true;
        }
    }
};

class Application {
public:
    void PushLayer(const LayerPtr& layer) { m_LayerStack.PushLayer(layer); }
    void PushOverlay(const LayerPtr& overlay) { m_LayerStack.PushOverlay(overlay); }

    void Run() {
        using clock = std::chrono::high_resolution_clock;
        auto lastTime = clock::now();
        bool running = true;
        int frames = 0;

        while (running && frames < 5) {
            auto now = clock::now();
            std::chrono::duration<float> elapsed = now - lastTime;
            float deltaTime = elapsed.count();
            lastTime = now;

            for (auto& layer : m_LayerStack.GetLayers()) {
                layer->OnUpdate(deltaTime);
            }

            Event e{"Click"};
            for (auto it = m_LayerStack.GetLayers().rbegin(); it != m_LayerStack.GetLayers().rend(); ++it) {
                (*it)->OnEvent(e);
                if (e.handled) break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            frames++;
        }
    }

private:
    LayerStack m_LayerStack;
};

int main() {
    Application app;

    auto renderLayer = std::make_shared<RenderingLayer>();
    auto uiLayer = std::make_shared<UILayer>();

    app.PushLayer(renderLayer);
    app.PushOverlay(uiLayer);

    app.Run();
    return 0;
}
