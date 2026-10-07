#include <iostream>
#include <string>

struct PrintJob {
    int id;
    std::string documentName;
    int pages;
};

class PrintQueue {
private:
    struct Node {
        PrintJob job;
        Node* next;

        explicit Node(const PrintJob& printJob)
            : job(printJob), next(nullptr) {}
    };

    Node* front = nullptr;
    Node* rear = nullptr;
    int jobCount = 0;

public:
    PrintQueue() = default;

    PrintQueue(const PrintQueue&) = delete;
    PrintQueue& operator=(const PrintQueue&) = delete;

    ~PrintQueue() {
        clear();
    }

    void addJob(const PrintJob& job) {
        Node* added = new Node(job);
        if (isEmpty()) {
            front = added;
        } else {
            rear->next = added;
        }
        rear = added;
        ++jobCount;
    }

    bool processJob() {
        if (isEmpty()) {
            std::cout << "Queue is empty. No job to process.\n";
            return false;
        }

        Node* processed = front;
        std::cout << "Processing Job " << processed->job.id << ": "
                  << processed->job.documentName << " ("
                  << processed->job.pages << " pages)\n";
        front = front->next;
        delete processed;
        --jobCount;

        if (front == nullptr) {
            rear = nullptr;
        }
        return true;
    }

    bool viewNextJob() const {
        if (isEmpty()) {
            std::cout << "Queue is empty. No next job.\n";
            return false;
        }

        std::cout << "Next Job: " << front->job.id << " - "
                  << front->job.documentName << " ("
                  << front->job.pages << " pages)\n";
        return true;
    }

    void displayQueue() const {
        if (isEmpty()) {
            std::cout << "Queue is empty.\n";
            return;
        }

        std::cout << "Waiting jobs (front to rear):\n";
        for (Node* current = front; current != nullptr; current = current->next) {
            std::cout << "Job " << current->job.id << " - "
                      << current->job.documentName << " ("
                      << current->job.pages << " pages)\n";
        }
    }

    int countJobs() const {
        return jobCount;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void clear() {
        while (front != nullptr) {
            Node* removed = front;
            front = front->next;
            delete removed;
        }
        rear = nullptr;
        jobCount = 0;
    }
};

int main() {
    PrintQueue queue;
    queue.addJob({101, "Assignment1.pdf", 10});
    queue.addJob({102, "Report.docx", 25});
    queue.addJob({103, "Notes.pdf", 5});
    queue.addJob({104, "LabTask.docx", 15});

    queue.displayQueue();
    std::cout << "Jobs waiting: " << queue.countJobs() << "\n\n";

    queue.processJob();
    queue.processJob();
    std::cout << '\n';

    queue.displayQueue();
    queue.addJob({105, "Presentation.pptx", 20});
    std::cout << '\n';

    queue.viewNextJob();
    std::cout << '\n';

    while (!queue.isEmpty()) {
        queue.processJob();
    }
    queue.processJob();

    return 0;
}