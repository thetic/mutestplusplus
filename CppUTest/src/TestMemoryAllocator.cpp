/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "CppUTest/TestMemoryAllocator.h"
#include "CppUTest/PlatformSpecificFunctions.h"
#include "CppUTest/TestHarness.h"

static char* checkedMalloc(size_t size)
{
    char* mem = reinterpret_cast<char*>(PlatformSpecificMalloc(size));
    if (mem == nullptr)
        FAIL("malloc returned null pointer");
    return mem;
}

static TestMemoryAllocator* currentNewAllocator = nullptr;
static TestMemoryAllocator* currentNewArrayAllocator = nullptr;
static TestMemoryAllocator* currentMallocAllocator = nullptr;

void setCurrentNewAllocator(TestMemoryAllocator* allocator)
{
    currentNewAllocator = allocator;
}

TestMemoryAllocator* getCurrentNewAllocator()
{
    if (currentNewAllocator == nullptr)
        setCurrentNewAllocatorToDefault();
    return currentNewAllocator;
}

void setCurrentNewAllocatorToDefault()
{
    currentNewAllocator = defaultNewAllocator();
}

TestMemoryAllocator* defaultNewAllocator()
{
    static TestMemoryAllocator allocator(
        "Standard New Allocator", "new", "delete"
    );
    return &allocator;
}

void setCurrentNewArrayAllocator(TestMemoryAllocator* allocator)
{
    currentNewArrayAllocator = allocator;
}

TestMemoryAllocator* getCurrentNewArrayAllocator()
{
    if (currentNewArrayAllocator == nullptr)
        setCurrentNewArrayAllocatorToDefault();
    return currentNewArrayAllocator;
}

void setCurrentNewArrayAllocatorToDefault()
{
    currentNewArrayAllocator = defaultNewArrayAllocator();
}

TestMemoryAllocator* defaultNewArrayAllocator()
{
    static TestMemoryAllocator allocator(
        "Standard New [] Allocator", "new []", "delete []"
    );
    return &allocator;
}

void setCurrentMallocAllocator(TestMemoryAllocator* allocator)
{
    currentMallocAllocator = allocator;
}

TestMemoryAllocator* getCurrentMallocAllocator()
{
    if (currentMallocAllocator == nullptr)
        setCurrentMallocAllocatorToDefault();
    return currentMallocAllocator;
}

void setCurrentMallocAllocatorToDefault()
{
    currentMallocAllocator = defaultMallocAllocator();
}

TestMemoryAllocator* defaultMallocAllocator()
{
    static TestMemoryAllocator allocator(
        "Standard Malloc Allocator", "malloc", "free"
    );
    return &allocator;
}

/////////////////////////////////////////////

TestMemoryAllocator::TestMemoryAllocator(
    const char* name_str, const char* alloc_name_str, const char* free_name_str
) :
    name_(name_str),
    alloc_name_(alloc_name_str),
    free_name_(free_name_str),
    hasBeenDestroyed_(false)
{
}

TestMemoryAllocator::~TestMemoryAllocator()
{
    hasBeenDestroyed_ = true;
}

bool TestMemoryAllocator::hasBeenDestroyed()
{
    return hasBeenDestroyed_;
}

bool TestMemoryAllocator::isOfEqualType(TestMemoryAllocator* allocator)
{
    return SimpleString::StrCmp(this->name(), allocator->name()) == 0;
}

char* TestMemoryAllocator::allocMemoryLeakNode(size_t size)
{
    return alloc_memory(size, "MemoryLeakNode", 1);
}

void TestMemoryAllocator::freeMemoryLeakNode(char* memory)
{
    free_memory(memory, 0, "MemoryLeakNode", 1);
}

char* TestMemoryAllocator::alloc_memory(size_t size, const char*, size_t)
{
    return checkedMalloc(size);
}

void TestMemoryAllocator::free_memory(char* memory, size_t, const char*, size_t)
{
    PlatformSpecificFree(memory);
}

const char* TestMemoryAllocator::name() const
{
    return name_;
}

const char* TestMemoryAllocator::alloc_name() const
{
    return alloc_name_;
}

const char* TestMemoryAllocator::free_name() const
{
    return free_name_;
}

TestMemoryAllocator* TestMemoryAllocator::actualAllocator()
{
    return this;
}

class LocationToFailAllocNode
{
public:
    int allocNumberToFail_;
    int actualAllocNumber_;
    const char* file_;
    size_t line_;
    LocationToFailAllocNode* next_;

    void failAtAllocNumber(int number, LocationToFailAllocNode* next)
    {
        init(next);
        allocNumberToFail_ = number;
    }

    void failNthAllocAt(
        int allocationNumber,
        const char* file,
        size_t line,
        LocationToFailAllocNode* next
    )
    {
        init(next);
        allocNumberToFail_ = allocationNumber;
        file_ = file;
        line_ = line;
    }

    bool shouldFail(int allocationNumber, const char* file, size_t line)
    {
        if (file_ && SimpleString::StrCmp(file, file_) == 0 && line == line_) {
            actualAllocNumber_++;
            return actualAllocNumber_ == allocNumberToFail_;
        }
        if (allocationNumber == allocNumberToFail_)
            return true;
        return false;
    }

private:
    void init(LocationToFailAllocNode* next = nullptr)
    {
        allocNumberToFail_ = 0;
        actualAllocNumber_ = 0;
        file_ = nullptr;
        line_ = 0;
        next_ = next;
    }
};

struct MemoryAccountantAllocationNode
{
    size_t size_;
    size_t allocations_;
    size_t deallocations_;
    size_t maxAllocations_;
    size_t currentAllocations_;
    MemoryAccountantAllocationNode* next_;
};

MemoryAccountantAllocationNode*
MemoryAccountant::createNewAccountantAllocationNode(
    size_t size, MemoryAccountantAllocationNode* next
) const
{
    MemoryAccountantAllocationNode* node =
        reinterpret_cast<MemoryAccountantAllocationNode*>(
            reinterpret_cast<void*>(allocator_->alloc_memory(
                sizeof(MemoryAccountantAllocationNode), __FILE__, __LINE__
            ))
        );
    node->size_ = size;
    node->allocations_ = 0;
    node->deallocations_ = 0;
    node->maxAllocations_ = 0;
    node->currentAllocations_ = 0;
    node->next_ = next;
    return node;
}

void MemoryAccountant::destroyAccountantAllocationNode(
    MemoryAccountantAllocationNode* node
) const
{
    allocator_->free_memory(
        reinterpret_cast<char*>(node), sizeof(*node), __FILE__, __LINE__
    );
}

MemoryAccountant::MemoryAccountant() :
    head_(nullptr),
    allocator_(defaultMallocAllocator()),
    useCacheSizes_(false)
{
}

MemoryAccountant::~MemoryAccountant()
{
    clear();
}

void MemoryAccountant::createCacheSizeNodes(size_t sizes[], size_t length)
{
    for (size_t i = 0; i < length; i++)
        findOrCreateNodeOfSize(sizes[i]);

    if (head_ == nullptr)
        head_ = createNewAccountantAllocationNode(0, nullptr);
    else {
        for (MemoryAccountantAllocationNode* lastNode = head_; lastNode;
             lastNode = lastNode->next_) {
            if (lastNode->next_ == nullptr) {
                lastNode->next_ = createNewAccountantAllocationNode(0, nullptr);
                break;
            }
        }
    }
}

void MemoryAccountant::useCacheSizes(size_t sizes[], size_t length)
{
    if (head_)
        FAIL(
            "MemoryAccountant: Cannot set cache sizes as allocations already "
            "occured!"
        );

    createCacheSizeNodes(sizes, length);
    useCacheSizes_ = true;
}

void MemoryAccountant::setAllocator(TestMemoryAllocator* allocator)
{
    allocator_ = allocator;
}

void MemoryAccountant::clear()
{
    MemoryAccountantAllocationNode* node = head_;
    MemoryAccountantAllocationNode* to_be_deleted = nullptr;
    while (node) {
        to_be_deleted = node;
        node = node->next_;
        destroyAccountantAllocationNode(to_be_deleted);
    }
    head_ = nullptr;
}

MemoryAccountantAllocationNode* MemoryAccountant::findNodeOfSize(size_t size
) const
{
    if (useCacheSizes_) {
        for (MemoryAccountantAllocationNode* node = head_; node;
             node = node->next_) {
            if (((size > node->size_) && (node->next_ == nullptr)) ||
                ((size <= node->size_) &&
                 !((node->next_->size_ != 0) && (node->next_->size_ <= size))))
                return node;
        }
    } else
        for (MemoryAccountantAllocationNode* node = head_; node;
             node = node->next_)
            if (node->size_ == size)
                return node;
    return nullptr;
}

MemoryAccountantAllocationNode*
MemoryAccountant::findOrCreateNodeOfSize(size_t size)
{
    if (useCacheSizes_)
        return findNodeOfSize(size);

    if (head_ && head_->size_ > size)
        head_ = createNewAccountantAllocationNode(size, head_);

    for (MemoryAccountantAllocationNode* node = head_; node;
         node = node->next_) {
        if (node->size_ == size)
            return node;
        if (node->next_ == nullptr || node->next_->size_ > size)
            node->next_ = createNewAccountantAllocationNode(size, node->next_);
    }
    head_ = createNewAccountantAllocationNode(size, head_);
    return head_;
}

void MemoryAccountant::alloc(size_t size)
{
    MemoryAccountantAllocationNode* node = findOrCreateNodeOfSize(size);
    node->allocations_++;
    node->currentAllocations_++;
    node->maxAllocations_ = (node->currentAllocations_ > node->maxAllocations_)
                                ? node->currentAllocations_
                                : node->maxAllocations_;
}

void MemoryAccountant::dealloc(size_t size)
{
    MemoryAccountantAllocationNode* node = findOrCreateNodeOfSize(size);
    node->deallocations_++;
    if (node->currentAllocations_)
        node->currentAllocations_--;
}

size_t MemoryAccountant::totalAllocationsOfSize(size_t size) const
{
    MemoryAccountantAllocationNode* node = findNodeOfSize(size);
    if (node)
        return node->allocations_;
    return 0;
}

size_t MemoryAccountant::totalDeallocationsOfSize(size_t size) const
{
    MemoryAccountantAllocationNode* node = findNodeOfSize(size);
    if (node)
        return node->deallocations_;
    return 0;
}

size_t MemoryAccountant::maximumAllocationAtATimeOfSize(size_t size) const
{
    MemoryAccountantAllocationNode* node = findNodeOfSize(size);
    if (node)
        return node->maxAllocations_;
    return 0;
}

size_t MemoryAccountant::totalAllocations() const
{
    size_t theTotalAllocations = 0;

    for (MemoryAccountantAllocationNode* node = head_; node; node = node->next_)
        theTotalAllocations += node->allocations_;

    return theTotalAllocations;
}

size_t MemoryAccountant::totalDeallocations() const
{
    size_t theTotalDeallocations = 0;

    for (MemoryAccountantAllocationNode* node = head_; node; node = node->next_)
        theTotalDeallocations += node->deallocations_;

    return theTotalDeallocations;
}

SimpleString MemoryAccountant::reportNoAllocations() const
{
    return SimpleString(
        "CppUTest Memory Accountant has not noticed any allocations or "
        "deallocations. Sorry\n"
    );
}

SimpleString MemoryAccountant::reportTitle() const
{
    if (useCacheSizes_)
        return "CppUTest Memory Accountant report (with cache sizes):\n";
    return "CppUTest Memory Accountant report:\n";
}

SimpleString MemoryAccountant::reportHeader() const
{
    if (useCacheSizes_)
        return "Cache size          # allocations    # deallocations   max # "
               "allocations at one time\n";
    return "Allocation size     # allocations    # deallocations   max # "
           "allocations at one time\n";
}

#define MEMORY_ACCOUNTANT_ROW_FORMAT                                           \
    "%s               %5d            %5d             %5d\n"

SimpleString MemoryAccountant::reportFooter() const
{
    return SimpleString("   Thank you for your business\n");
}

SimpleString MemoryAccountant::stringSize(size_t size) const
{
    return (size == 0) ? StringFrom("other")
                       : StringFromFormat("%5d", static_cast<int>(size));
}

SimpleString MemoryAccountant::report() const
{
    if (head_ == nullptr)
        return reportNoAllocations();

    SimpleString accountantReport = reportTitle() + reportHeader();

    for (MemoryAccountantAllocationNode* node = head_; node; node = node->next_)
        accountantReport += StringFromFormat(
            MEMORY_ACCOUNTANT_ROW_FORMAT,
            stringSize(node->size_).asCharString(),
            static_cast<int>(node->allocations_),
            static_cast<int>(node->deallocations_),
            static_cast<int>(node->maxAllocations_)
        );

    return accountantReport + reportFooter();
}

AccountingTestMemoryAllocator::AccountingTestMemoryAllocator(
    MemoryAccountant& accountant, TestMemoryAllocator* origAllocator
) :
    accountant_(accountant),
    originalAllocator_(origAllocator),
    head_(nullptr)
{
}

AccountingTestMemoryAllocator::~AccountingTestMemoryAllocator() {}

struct AccountingTestMemoryAllocatorMemoryNode
{
    char* memory_;
    size_t size_;
    AccountingTestMemoryAllocatorMemoryNode* next_;
};

void AccountingTestMemoryAllocator::addMemoryToMemoryTrackingToKeepTrackOfSize(
    char* memory, size_t size
)
{
    AccountingTestMemoryAllocatorMemoryNode* node =
        reinterpret_cast<AccountingTestMemoryAllocatorMemoryNode*>(
            reinterpret_cast<void*>(originalAllocator_->alloc_memory(
                sizeof(AccountingTestMemoryAllocatorMemoryNode), __FILE__,
                __LINE__
            ))
        );
    node->memory_ = memory;
    node->size_ = size;
    node->next_ = head_;
    head_ = node;
}

size_t AccountingTestMemoryAllocator::removeNextNodeAndReturnSize(
    AccountingTestMemoryAllocatorMemoryNode* node
)
{
    AccountingTestMemoryAllocatorMemoryNode* foundNode = node->next_;
    node->next_ = node->next_->next_;

    size_t size = foundNode->size_;
    originalAllocator_->free_memory(
        reinterpret_cast<char*>(foundNode), size, __FILE__, __LINE__
    );
    return size;
}

size_t AccountingTestMemoryAllocator::removeHeadAndReturnSize()
{
    AccountingTestMemoryAllocatorMemoryNode* foundNode = head_;
    head_ = head_->next_;

    size_t size = foundNode->size_;
    originalAllocator_->free_memory(
        reinterpret_cast<char*>(foundNode), size, __FILE__, __LINE__
    );
    return size;
}

size_t
AccountingTestMemoryAllocator::removeMemoryFromTrackingAndReturnAllocatedSize(
    char* memory
)
{
    if (head_ && head_->memory_ == memory)
        return removeHeadAndReturnSize();

    for (AccountingTestMemoryAllocatorMemoryNode* node = head_; node;
         node = node->next_) {
        if (node->next_ && node->next_->memory_ == memory)
            return removeNextNodeAndReturnSize(node);
    }

    return 0;
}

char* AccountingTestMemoryAllocator::alloc_memory(
    size_t size, const char* file, size_t line
)
{
    accountant_.alloc(size);
    char* memory = originalAllocator_->alloc_memory(size, file, line);
    addMemoryToMemoryTrackingToKeepTrackOfSize(memory, size);
    return memory;
}

void AccountingTestMemoryAllocator::free_memory(
    char* memory, size_t, const char* file, size_t line
)
{
    size_t size = removeMemoryFromTrackingAndReturnAllocatedSize(memory);
    accountant_.dealloc(size);
    originalAllocator_->free_memory(memory, size, file, line);
}

TestMemoryAllocator* AccountingTestMemoryAllocator::actualAllocator()
{
    return originalAllocator_->actualAllocator();
}

TestMemoryAllocator* AccountingTestMemoryAllocator::originalAllocator()
{
    return originalAllocator_;
}

const char* AccountingTestMemoryAllocator::alloc_name() const
{
    return originalAllocator_->alloc_name();
}

const char* AccountingTestMemoryAllocator::free_name() const
{
    return originalAllocator_->free_name();
}
