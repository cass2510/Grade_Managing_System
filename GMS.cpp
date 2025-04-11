#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

// 출력 결과를 저장할 텍스트 파일 (Grade.txt). ios::trunc: 기존 파일 내용 초기화
ofstream logFile("Grade.txt", ios::out | ios::trunc);

// 문자열 형태의 성적(A+, B 등급)을 환산 점수(double)로 변환하는 함수
double gradeToPoint(const string& gradeStr) {
    // 정적 맵: 등급 -> 점수
    static map<string, double> gradeMap = {
        {"A+", 4.5}, {"A", 4.0}, {"B+", 3.5}, {"B", 3.0},
        {"C+", 2.5}, {"C", 2.0}, {"D+", 1.5}, {"D", 1.0}, {"F", 0.0}
    };
    auto it = gradeMap.find(gradeStr);
    return (it != gradeMap.end()) ? it->second : -1.0;
}

// 템플릿 클래스 Subject
// 학점(int) 외에 다른 타입(T)을 확장할 수 있도록 템플릿 사용
template <typename T>
class Subject {
private:
    string name;   // 과목명
    T credit;      // 학점 (T는 일반적으로 int로 사용됨)
public:
    Subject(string name, T credit) : name(name), credit(credit) {}
    string getName() const { return name; }
    T getCredit() const { return credit; }
};

// Grade 클래스: 한 과목에 대한 점수 정보를 저장
class Grade {
private:
    Subject<int> subject;  // 과목 정보 (학점 포함)
    double point;          // 환산 점수
public:
    Grade(Subject<int> subject, double point) : subject(subject), point(point) {}
    double getPoint() const { return point; }
    Subject<int> getSubject() const { return subject; }
};

// Person 추상 클래스: 이름 필드와 displayInfo() 순수 가상 함수 포함
class Person {
protected:
    string name;
public:
    Person(string name) : name(name) {}
    string getName() const { return name; }
    virtual void displayInfo() const = 0; // 반드시 자식 클래스에서 구현해야 함
};

// Student 클래스: Person을 상속받고, 학번 및 Grade 정보를 가짐
class Student : public Person {
private:
    int studentId;
    vector<Grade> grades;  // 학생의 과목별 성적 리스트
public:
    Student(string name, int studentId) : Person(name), studentId(studentId) {}

    // 성적 추가 함수
    void addGrade(const Subject<int>& subject, double point) {
        grades.push_back(Grade(subject, point));
    }

    // 평균 환산 점수 계산 (전체 학점 대비)
    double calculateAverage() const {
        double totalPoints = 0;
        int totalCredits = 0;
        for (const auto& grade : grades) {
            totalPoints += grade.getPoint() * grade.getSubject().getCredit();
            totalCredits += grade.getSubject().getCredit();
        }
        return totalCredits == 0 ? 0 : totalPoints / totalCredits;
    }

    // 총 이수 학점 계산
    int calculateTotalCredits() const {
        int totalCredits = 0;
        for (const auto& grade : grades) {
            totalCredits += grade.getSubject().getCredit();
        }
        return totalCredits;
    }

    // 학생의 전체 성적 리스트 반환
    vector<Grade> getGrades() const {
        return grades;
    }

    // 학생 평균 점수를 기준으로 정렬 (내림차순)
    bool operator<(const Student& other) const {
        return this->calculateAverage() > other.calculateAverage();
    }

    // 학생 정보와 성적 출력 함수 (화면 + 파일)
    void displayInfo() const override {
        cout << "\n===========================================\n";
        cout << "학생 이름: " << name << " (학번: " << studentId << ")\n";
        cout << "-------------------------------------------\n";
        cout << "| 과목명         | 학점   | 환산 점수   |\n";
        cout << "-------------------------------------------\n";

        logFile << "\n===========================================\n";
        logFile << "학생 이름: " << name << " (학번: " << studentId << ")\n";
        logFile << "-------------------------------------------\n";
        logFile << "| 과목명         | 학점   | 환산 점수   |\n";
        logFile << "-------------------------------------------\n";

        for (const auto& grade : grades) {
            cout << "| " << setw(13) << left << grade.getSubject().getName()
                << "| " << setw(7) << left << grade.getSubject().getCredit()
                << "| " << setw(12) << fixed << setprecision(2) << grade.getPoint() << "|\n";

            logFile << "| " << setw(14) << left << grade.getSubject().getName()
                << "| " << setw(8) << left << grade.getSubject().getCredit()
                << "| " << setw(13) << fixed << setprecision(2) << grade.getPoint() << "|\n";
        }

        cout << "-------------------------------------------\n";
        cout << "▶ 학점 평균: " << fixed << setprecision(2) << calculateAverage() << "\n";
        cout << "▶ 이수 학점: " << calculateTotalCredits() << "학점\n";
        cout << "===========================================\n";

        logFile << "-------------------------------------------\n";
        logFile << "▶ 학점 평균: " << fixed << setprecision(2) << calculateAverage() << "\n";
        logFile << "▶ 이수 학점: " << calculateTotalCredits() << "학점\n";
        logFile << "===========================================\n";
    }
};

// Classroom 클래스: 학생 리스트, 과목별 순위, 통계 출력 등의 기능 제공
class Classroom {
private:
    vector<Student> students;
    map<string, vector<Subject<int>>> subjectMap;  // 과목별 정보 저장
public:
    void addStudent(const Student& s) {
        students.push_back(s);
    }

    void addSubject(const string& name, int credit) {
        subjectMap[name].push_back(Subject<int>(name, credit));
    }

    // 전체 학생들의 평균, 최고/최저 성적 출력
    void printStats() {
        double total = 0, highest = 0, lowest = 4.5;
        for (const auto& s : students) {
            double avg = s.calculateAverage();
            total += avg;
            if (avg > highest) highest = avg;
            if (avg < lowest) lowest = avg;
        }

        cout << "\n▣ [학과 전체 통계]\n";
        cout << "-------------------------------------------\n";
        cout << "학과 전체 학점 평균 : " << fixed << setprecision(2) << (total / students.size()) << "\n";
        cout << "최고 학점 평균: " << highest << "\n";
        cout << "최저 학점 평균: " << lowest << "\n";
        cout << "-------------------------------------------\n";

        logFile << "\n▣ [학과 전체 통계]\n";
        logFile << "-------------------------------------------\n";
        logFile << "학과 전체 학점 평균 : " << fixed << setprecision(2) << (total / students.size()) << "\n";
        logFile << "최고 학점 평균: " << highest << "\n";
        logFile << "최저 학점 평균: " << lowest << "\n";
        logFile << "-------------------------------------------\n";
    }

    // 평균 점수 상위 3명 출력
    void printTop3() {
        sort(students.begin(), students.end()); // operator< 오버로드 활용
        cout << "\n▣ [학점 평균 상위 3명]\n";
        cout << "-------------------------------------------\n";
        for (int i = 0; i < 3 && i < students.size(); ++i) {
            cout << i + 1 << "위: " << students[i].getName()
                << " - 학점 평균: " << fixed << setprecision(2) << students[i].calculateAverage() << "\n";

            logFile << i + 1 << "위: " << students[i].getName()
                << " - 학점 평균: " << fixed << setprecision(2) << students[i].calculateAverage() << "\n";
        }
        cout << "-------------------------------------------\n";
        logFile << "-------------------------------------------\n";
    }

    // 과목별 성적 순위 출력
    void printSubjectRankings() {
        for (auto& subject : subjectMap) {
            vector<pair<string, double>> subjectScores;
            for (const auto& student : students) {
                for (const auto& grade : student.getGrades()) {
                    if (grade.getSubject().getName() == subject.first) {
                        subjectScores.push_back({ student.getName(), grade.getPoint() });
                    }
                }
            }

            // 내림차순 정렬
            sort(subjectScores.begin(), subjectScores.end(),
                [](const pair<string, double>& a, const pair<string, double>& b) {
                    return a.second > b.second;
                });

            cout << "\n▣ [" << subject.first << " 과목별 순위]\n";
            cout << "-------------------------------------------\n";
            logFile << "\n▣ [" << subject.first << " 과목별 순위]\n";
            logFile << "-------------------------------------------\n";

            int rank = 1;
            for (const auto& score : subjectScores) {
                cout << rank << "위: " << score.first
                    << " - 환산 점수: " << fixed << setprecision(2) << score.second << "\n";
                logFile << rank << "위: " << score.first
                    << " - 환산 점수: " << fixed << setprecision(2) << score.second << "\n";
                rank++;
            }
            cout << "-------------------------------------------\n";
            logFile << "-------------------------------------------\n";
        }
    }

    // 모든 학생 정보 출력
    void showAllStudents() {
        for (const auto& s : students) {
            s.displayInfo();
        }
    }
};

// 메인 함수: 학생 수 입력 → 정보 등록 → 전체 통계 및 순위 출력
int main() {
    Classroom classroom;

    int studentCount;
    cout << "▶ 학생 수를 입력하세요: ";
    cin >> studentCount;

    for (int i = 0; i < studentCount; ++i) {
        string name;
        int id;
        cout << "\n▶ 학생 #" << i + 1 << "의 이름과 학번을 입력하세요 (예: 김철수 20241234): ";
        cin >> name >> id;

        Student s(name, id);

        int subjectCount;
        cout << " - 등록할 과목 수: ";
        cin >> subjectCount;

        for (int j = 0; j < subjectCount; ++j) {
            string subName, gradeStr;
            int credit;

            cout << " - 과목명, 학점, 등급을 입력하세요 (예: C++프로그래밍 3 A+): ";
            cin >> subName >> credit >> gradeStr;

            double point = gradeToPoint(gradeStr);
            while (point < 0) {
                cout << " ▣ 올바르지 않은 성적 등급입니다. 다시 입력하세요: ";
                cin >> gradeStr;
                point = gradeToPoint(gradeStr);
            }

            classroom.addSubject(subName, credit);
            s.addGrade(Subject<int>(subName, credit), point);
        }

        classroom.addStudent(s);
    }

    // 통계 및 출력
    classroom.printStats();
    classroom.printTop3();
    classroom.printSubjectRankings();
    classroom.showAllStudents();

    logFile.close();  // 파일 닫기

    char openFile;
    cout << "\n▶ 출력된 내용을 텍스트 파일로 확인하시겠습니까? (Y/N): ";
    cin >> openFile;

    if (openFile == 'Y' || openFile == 'y') {
        system("start Grade.txt");  // Windows에서 기본 텍스트 편집기로 파일 열기
    }

    return 0;
}
