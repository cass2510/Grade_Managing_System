#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>

using namespace std;

// 출력 로그 파일 설정 (UTF-8, 덮어쓰기 모드)
ofstream logFile("Grade.txt", ios::out | ios::trunc); 

// 성적 등급을 점수(평점)으로 변환하는 함수
double gradeToPoint(const string& gradeStr) {
    static map<string, double> gradeMap = {
        {"A+", 4.5}, {"A", 4.0}, {"B+", 3.5}, {"B", 3.0},
        {"C+", 2.5}, {"C", 2.0}, {"D+", 1.5}, {"D", 1.0}, {"F", 0.0}
    };
    auto it = gradeMap.find(gradeStr);
    return (it != gradeMap.end()) ? it->second : -1.0;
}

// 과목을 나타내는 템플릿 클래스 (학점 타입을 일반화)
template <typename T>
class Subject {
private:
    string name;  // 과목명
    T credit;     // 학점
public:
    Subject(string name, T credit) : name(name), credit(credit) {}
    string getName() const { return name; }
    T getCredit() const { return credit; }
};

// 학생의 성적 정보를 나타내는 클래스
class Grade {
private:
    Subject<int> subject;  // 과목 정보
    double point;          // 환산 점수 (평점)
public:
    Grade(Subject<int> subject, double point) : subject(subject), point(point) {}
    double getPoint() const { return point; }
    Subject<int> getSubject() const { return subject; }
};

// 사람의 기본 정보를 나타내는 추상 클래스
class Person {
protected:
    string name;  // 이름
public:
    Person(string name) : name(name) {}
    string getName() const { return name; }
    virtual void displayInfo() const = 0;  // 순수가상함수 (자식 클래스에서 구현 필요)
};

// 학생 정보를 관리하는 클래스
class Student : public Person {
private:
    int studentId;              // 학번
    vector<Grade> grades;       // 수강한 과목들의 성적 정보
public:
    Student(string name, int studentId) : Person(name), studentId(studentId) {}

    // 성적 추가
    void addGrade(const Subject<int>& subject, double point) {
        grades.push_back(Grade(subject, point));
    }

    // 평균 평점 계산
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

    vector<Grade> getGrades() const {
        return grades;
    }

    // 정렬을 위한 연산자 오버로딩 (내림차순: 높은 평점이 먼저)
    bool operator<(const Student& other) const {
        return this->calculateAverage() > other.calculateAverage();
    }

    // 학생 정보 출력 (콘솔 및 파일)
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

// 학급 전체를 관리하는 클래스
class Classroom {
private:
    vector<Student> students;                                // 학생 리스트
    map<string, vector<Subject<int>>> subjectMap;            // 과목별 정보 저장용
public:
    void addStudent(const Student& s) {
        students.push_back(s);
    }

    void addSubject(const string& name, int credit) {
        subjectMap[name].push_back(Subject<int>(name, credit));
    }

    // 전체 학생들의 평균, 최고/최저 학점 출력
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

    // 학점 평균 상위 3명의 학생 정보 출력
    void printTop3() {
        sort(students.begin(), students.end());  // 평균 기준 정렬
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

    // 과목별 순위 출력 (성적 높은 순으로)
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

    // 전체 학생 정보 출력
    void showAllStudents() {
        for (const auto& s : students) {
            s.displayInfo();
        }
    }
};

int main() {
    Classroom classroom;

    // 학생 수 입력
    int studentCount;
    cout << "▶ 학생 수를 입력하세요: ";
    cin >> studentCount;

    // 학생 정보 및 성적 입력
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

            // 등급 유효성 검사
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

    // 출력 기능 호출
    classroom.printStats();
    classroom.printTop3();
    classroom.printSubjectRankings();
    classroom.showAllStudents();

    logFile.close();

    // 출력 파일 열기 여부 선택
    char openFile;
    cout << "\n▶ 출력된 내용을 텍스트 파일로 확인하시겠습니까? (Y/N): ";
    cin >> openFile;

    if (openFile == 'Y' || openFile == 'y') {
        system("start Grade.txt");  // Windows 기본 텍스트 편집기로 열기
    }

    return 0;
}
