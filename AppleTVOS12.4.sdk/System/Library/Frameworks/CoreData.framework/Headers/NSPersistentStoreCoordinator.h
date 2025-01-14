/*
    NSPersistentStoreCoordinator.h
    Core Data
    Copyright (c) 2004-2018, Apple Inc.
    All rights reserved.
*/

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSSet.h>
#import <Foundation/NSError.h>
#import <Foundation/NSLock.h>
#import <CoreData/CoreDataDefines.h>

NS_ASSUME_NONNULL_BEGIN

@class NSURL;
@class NSValue;
@class NSManagedObjectID;
@class NSManagedObjectModel;
@class NSManagedObjectContext;
@class NSPersistentStore;
@class NSPersistentStoreRequest;
@class NSPersistentStoreDescription;

// Persistent store types supported by Core Data:
COREDATA_EXTERN NSString * const NSSQLiteStoreType API_AVAILABLE(macosx(10.4),ios(3.0));
COREDATA_EXTERN NSString * const NSXMLStoreType API_AVAILABLE(macosx(10.4)) API_UNAVAILABLE(ios);
COREDATA_EXTERN NSString * const NSBinaryStoreType API_AVAILABLE(macosx(10.4),ios(3.0));
COREDATA_EXTERN NSString * const NSInMemoryStoreType API_AVAILABLE(macosx(10.4),ios(3.0));

// Persistent store metadata dictionary keys:

// key in the metadata dictionary to identify the store type
COREDATA_EXTERN NSString * const NSStoreTypeKey API_AVAILABLE(macosx(10.4),ios(3.0));    

// key in the metadata dictionary to identify the store UUID - the store UUID is useful to identify stores through URI representations, but it is NOT guaranteed to be unique (while the UUID generated for new stores is unique, users can freely copy files and thus the UUID stored inside, so developers that track/reference stores explicitly do need to be aware of duplicate UUIDs and potentially override the UUID when a new store is added to the list of known stores in their application)
COREDATA_EXTERN NSString * const NSStoreUUIDKey API_AVAILABLE(macosx(10.4),ios(3.0));    

/* A notification posted before the list of open persistent stores changes, similar to NSPersistentStoreCoordinatorStoresDidChangeNotification.  If the application is running, Core Data will post this before responding to iCloud account changes or "Delete All" from Documents & Data.
 */
COREDATA_EXTERN NSString * const NSPersistentStoreCoordinatorStoresWillChangeNotification API_AVAILABLE(macosx(10.9),ios(7.0));

// user info dictionary contains information about the stores that were added or removed
COREDATA_EXTERN NSString * const NSPersistentStoreCoordinatorStoresDidChangeNotification API_AVAILABLE(macosx(10.4),ios(3.0));    

// sent during the invocation of NSPersistentStore's willRemoveFromPersistentStoreCoordinator during store deallocation or removal
COREDATA_EXTERN NSString * const NSPersistentStoreCoordinatorWillRemoveStoreNotification API_AVAILABLE(macosx(10.5),ios(3.0));    

// User info keys for NSPersistentStoreCoordinatorStoresDidChangeNotification:

// The object values for NSAddedPersistentStoresKey and NSRemovedPersistentStoresKey will be arrays containing added/removed stores
COREDATA_EXTERN NSString * const NSAddedPersistentStoresKey API_AVAILABLE(macosx(10.4),ios(3.0));
COREDATA_EXTERN NSString * const NSRemovedPersistentStoresKey API_AVAILABLE(macosx(10.4),ios(3.0));

// The object value for NSUUIDChangedPersistentStoresKey will be an array where the object at index 0 will be the old store instance, and the object at index 1 the new
COREDATA_EXTERN NSString * const NSUUIDChangedPersistentStoresKey API_AVAILABLE(macosx(10.4),ios(3.0));

// Persistent store option keys:

// flag indicating whether a store is treated as read-only or not - default is NO
COREDATA_EXTERN NSString * const NSReadOnlyPersistentStoreOption API_AVAILABLE(macosx(10.4),ios(3.0));    

// flag indicating whether an XML file should be validated with the DTD while opening - default is NO
COREDATA_EXTERN NSString * const NSValidateXMLStoreOption API_AVAILABLE(macosx(10.4)) API_UNAVAILABLE(ios);    

// Migration keys:

/* Options key specifying the connection timeout for Core Data stores.  This value (an NSNumber) represents the duration, in seconds, Core Data will wait while attempting to create a connection to a persistent store.  If a connection is unable to be made within that timeframe, the operation is aborted and an error is returned.  
*/
COREDATA_EXTERN NSString * const NSPersistentStoreTimeoutOption API_AVAILABLE(macosx(10.5),ios(3.0));

/* Options key for a dictionary of sqlite pragma settings with pragma values indexed by pragma names as keys.  All pragma values must be specified as strings.  The fullfsync and synchronous pragmas control the tradeoff between write performance (write to disk speed & cache utilization) and durability (data loss/corruption sensitivity to power interruption).  For more information on pragma settings visit <http://sqlite.org/pragma.html>
*/
COREDATA_EXTERN NSString * const NSSQLitePragmasOption API_AVAILABLE(macosx(10.5),ios(3.0));

/* Option key to run an analysis of the store data to optimize indices based on statistical information when the store is added to the coordinator.  This invokes SQLite's ANALYZE command.  Ignored by other stores.
*/
COREDATA_EXTERN NSString * const NSSQLiteAnalyzeOption API_AVAILABLE(macosx(10.5),ios(3.0));

/* Option key to rebuild the store file, forcing a database wide defragmentation when the store is added to the coordinator.  This invokes SQLite's VACUUM command.  Ignored by other stores.
 */
COREDATA_EXTERN NSString * const NSSQLiteManualVacuumOption API_AVAILABLE(macosx(10.6),ios(3.0));

/* Options key to ignore the built-in versioning provided by Core Data.  If the value for this key (an NSNumber) evaluates to YES (using boolValue), Core Data will not compare the version hashes between the managed object model in the coordinator and the metadata for the loaded store.  (It will, however, continue to update the version hash information in the metadata.)  This key is specified by default for all applications linked on or before Mac OS X 10.4.
*/
COREDATA_EXTERN NSString * const NSIgnorePersistentStoreVersioningOption API_AVAILABLE(macosx(10.5),ios(3.0));

/* Options key to automatically attempt to migrate versioned stores.  If the value for this key (an NSNumber) evaluates to YES (using boolValue) Core Data will, if the version hash information for added store is determined to be incompatible with the model for the coordinator, attempt to locate the source and mapping models in the application bundles, and perform a migration.  
*/
COREDATA_EXTERN NSString * const NSMigratePersistentStoresAutomaticallyOption API_AVAILABLE(macosx(10.5),ios(3.0));

/* When combined with NSMigratePersistentStoresAutomaticallyOption, coordinator will attempt to infer a mapping model if none can be found */
COREDATA_EXTERN NSString * const NSInferMappingModelAutomaticallyOption API_AVAILABLE(macosx(10.6),ios(3.0));

/* Key to represent the version hash information (dictionary) for the model used to create a persistent store.  This key is used in the metadata for a persistent store.
*/
COREDATA_EXTERN NSString * const NSStoreModelVersionHashesKey API_AVAILABLE(macosx(10.5),ios(3.0));    

/* Key to represent the version identifier for the model used to create the store. This key is used in the metadata for a persistent store.
*/
COREDATA_EXTERN NSString * const NSStoreModelVersionIdentifiersKey API_AVAILABLE(macosx(10.5),ios(3.0));    

/* Key to represent the earliest version of MacOS X the persistent store should support.  Backward compatibility may preclude some features.  The numeric values are defined in AvailabilityMacros.h
*/
COREDATA_EXTERN NSString * const NSPersistentStoreOSCompatibility API_AVAILABLE(macosx(10.5),ios(3.0));    

/* User info key specifying the maximum connection pool size that should be used on a store that supports concurrent request handling, the value should be an NSNumber. The connection pool size determines the number of requests a store can handle concurrently, and should be a function of how many contexts are attempting to access store data at any time. Generally, application developers should not set this, and should use the default value. The default connection pool size is implementation dependent and may vary by store type and/or platform.
 */
COREDATA_EXTERN NSString * const NSPersistentStoreConnectionPoolMaxSizeKey API_AVAILABLE(macosx(10.12),ios(10.0),tvos(10.0),watchos(3.0));

/* Spotlight indexing and external record support keys */

COREDATA_EXTERN NSString * const NSCoreDataCoreSpotlightExporter API_AVAILABLE(macosx(10.13),ios(11.0)) API_UNAVAILABLE(tvos,watchos);


/* store option for the destroy... and replace... to indicate that the store file should be destroyed even if the operation might be unsafe (overriding locks
 */
COREDATA_EXTERN NSString * const NSPersistentStoreForceDestroyOption API_AVAILABLE(macosx(10.8),ios(6.0));

/* Key to represent the protection class for the persistent store.  Backward compatibility may preclude some features.  The acceptable values are those defined in Foundation for the NSFileProtectionKey.  The default value of NSPersistentStoreFileProtectionKey is NSFileProtectionCompleteUntilFirstUserAuthentication for all applications built on or after iOS5.  The default value for all older applications is NSFileProtectionNone. */
COREDATA_EXTERN NSString * const NSPersistentStoreFileProtectionKey API_AVAILABLE(ios(5.0)) API_UNAVAILABLE(macosx);

/* Dictionary key for enabling persistent history - default is NO */
COREDATA_EXTERN NSString * const NSPersistentHistoryTrackingKey API_AVAILABLE(macosx(10.13),ios(11.0),tvos(11.0),watchos(4.0));

/*
 Allows developers to provide an additional set of classes (which must implement NSSecureCoding) that should be used while
 decoding a binary store.
 Using this option is preferable to using NSBinaryStoreInsecureDecodingCompatibilityOption.
 */
COREDATA_EXTERN NSString * const NSBinaryStoreSecureDecodingClasses API_AVAILABLE(macosx(10.13),ios(11.0),tvos(11.0),watchos(4.0));

/*
 Indicate that the binary store should be decoded insecurely. This may be necessary if a store has metadata or transformable
 properties containing non-standard classes. If possible, developers should use the NSBinaryStoreSecureDecodingClasses option
 to specify the contained classes, allowing the binary store to to be securely decoded.
 Applications linked before the availability date will default to using this option.
 */
COREDATA_EXTERN NSString * const NSBinaryStoreInsecureDecodingCompatibilityOption API_AVAILABLE(macosx(10.13),ios(11.0),tvos(11.0),watchos(4.0));

API_AVAILABLE(macosx(10.4),ios(3.0))
@interface NSPersistentStoreCoordinator : NSObject {
}

- (instancetype)initWithManagedObjectModel:(NSManagedObjectModel *)model NS_DESIGNATED_INITIALIZER;

@property (readonly, strong) NSManagedObjectModel *managedObjectModel;

@property (readonly, strong) NSArray<__kindof NSPersistentStore *> *persistentStores;

/* custom name for a coordinator.  Coordinators will set the label on their queue */
@property (nullable, copy) NSString *name  API_AVAILABLE(macosx(10.10),ios(8.0));

- (nullable __kindof NSPersistentStore *)persistentStoreForURL:(NSURL *)URL;
- (NSURL *)URLForPersistentStore:(NSPersistentStore *)store;

/* Sets the URL for the specified store in the coordinator.  For atomic stores, this will alter the location to which the next save operation will persist the file;  for non-atomic stores, invoking this method will release the existing connection and create a new one at the specified URL.  (For non-atomic stores, a store must pre-exist at the destination URL; a new store will not be created.)
 */
- (BOOL)setURL:(NSURL*)url forPersistentStore:(NSPersistentStore *)store API_AVAILABLE(macosx(10.5),ios(3.0));

/* Adds the store at the specified URL (of the specified type) to the coordinator with the model configuration and options.  The configuration can be nil -- then it's the complete model; storeURL is usually the file location of the database
 */
- (nullable __kindof NSPersistentStore *)addPersistentStoreWithType:(NSString *)storeType configuration:(nullable NSString *)configuration URL:(nullable NSURL *)storeURL options:(nullable NSDictionary *)options error:(NSError **)error;

- (void)addPersistentStoreWithDescription:(NSPersistentStoreDescription *)storeDescription completionHandler:(void (^)(NSPersistentStoreDescription *, NSError * _Nullable))block API_AVAILABLE(macosx(10.12),ios(10.0),tvos(10.0),watchos(3.0));

- (BOOL)removePersistentStore:(NSPersistentStore *)store error:(NSError **)error;

/* Sets the metadata stored in the persistent store during the next save operation executed on it; the store type and UUID (NSStoreTypeKey and NSStoreUUIDKey) are always added automatically (but NSStoreUUIDKey is only added if it is not set manually as part of the dictionary argument)
 */
- (void)setMetadata:(nullable NSDictionary<NSString *, id> *)metadata forPersistentStore:(NSPersistentStore *)store;

/* Returns the metadata currently stored or to-be-stored in the persistent store
 */
- (NSDictionary<NSString *, id> *)metadataForPersistentStore:(NSPersistentStore *)store;

/* Given a URI representation of an object ID, returns an object ID if a matching store is available or nil if a matching store cannot be found (the URI representation contains a UUID of the store the ID is coming from, and the coordinator can match it against the stores added to it)
 */
- (nullable NSManagedObjectID *)managedObjectIDForURIRepresentation:(NSURL *)url;

/* Sends a request to all of the stores associated with this coordinator.
 Returns an array if successful,  nil if not.
 The contents of the array will vary depending on the request type: NSFetchRequest results will be an array of managed objects, managed object IDs, or NSDictionaries;
 NSSaveChangesRequests will an empty array. User defined requests will return arrays of arrays, where the nested array is the result returned form a single store.
 */
- (nullable id)executeRequest:(NSPersistentStoreRequest *)request withContext:(NSManagedObjectContext *)context error:(NSError**)error API_AVAILABLE(macosx(10.7),ios(5.0));

/* Returns a dictionary of the registered store types:  the keys are the store type strings and the values are the NSPersistentStore subclasses wrapped in NSValues.
*/
@property(class, readonly, strong) NSDictionary<NSString *, NSValue *> *registeredStoreTypes API_AVAILABLE(macosx(10.5),ios(3.0));

/* Registers the specified NSPersistentStore subclass for the specified store type string.  This method must be invoked before a custom subclass of NSPersistentStore can be loaded into a persistent store coordinator.  Passing nil for the store class argument will unregister the specified store type.
*/
+ (void)registerStoreClass:(Class _Nullable)storeClass forStoreType:(NSString *)storeType API_AVAILABLE(macosx(10.5),ios(3.0));

/* Allows to access the metadata stored in a persistent store without warming up a CoreData stack; the guaranteed keys in this dictionary are NSStoreTypeKey and NSStoreUUIDKey. If storeType is nil, Core Data will guess which store class should be used to get/set the store file's metadata. 
 */
+ (nullable NSDictionary<NSString *, id> *)metadataForPersistentStoreOfType:(NSString*)storeType URL:(NSURL *)url options:(nullable NSDictionary *)options error:(NSError **)error API_AVAILABLE(macosx(10.9),ios(7.0));
+ (BOOL)setMetadata:(nullable NSDictionary<NSString *, id> *)metadata forPersistentStoreOfType:(NSString*)storeType URL:(NSURL*)url options:(nullable NSDictionary*)options error:(NSError**)error API_AVAILABLE(macosx(10.9),ios(7.0));

    
/* Used for save as - performance may vary depending on the type of old and new store; the old store is usually removed from the coordinator by the migration operation, and therefore is no longer a useful reference after invoking this method 
*/
- (nullable NSPersistentStore *)migratePersistentStore:(NSPersistentStore *)store toURL:(NSURL *)URL options:(nullable NSDictionary *)options withType:(NSString *)storeType error:(NSError **)error;

/* delete or truncate the target persistent store in accordance with the store class's requirements.  It is important to pass similar options as addPersistentStoreWithType: ... SQLite stores will honor file locks, journal files, journaling modes, and other intricacies.  It is not possible to unlink a database file safely out from underneath another thread or process, so this API performs a truncation.  Other stores will default to using NSFileManager.
 */
- (BOOL)destroyPersistentStoreAtURL:(NSURL *)url withType:(NSString *)storeType options:(nullable NSDictionary *)options error:(NSError**)error API_AVAILABLE(macosx(10.11),ios(9.0));

/* copy or overwrite the target persistent store in accordance with the store class's requirements.  It is important to pass similar options as addPersistentStoreWithType: ... SQLite stores will honor file locks, journal files, journaling modes, and other intricacies.  Other stores will default to using NSFileManager.
 */
- (BOOL)replacePersistentStoreAtURL:(NSURL *)destinationURL destinationOptions:(nullable NSDictionary *)destinationOptions withPersistentStoreFromURL:(NSURL *)sourceURL sourceOptions:(nullable NSDictionary *)sourceOptions storeType:(NSString *)storeType error:(NSError**)error API_AVAILABLE(macosx(10.11),ios(9.0));

/* asynchronously performs the block on the coordinator's queue.  Encapsulates an autorelease pool. */
- (void)performBlock:(void (^)(void))block  API_AVAILABLE(macosx(10.10),ios(8.0));

/* synchronously performs the block on the coordinator's queue.  May safely be called reentrantly. Encapsulates an autorelease pool. */
- (void)performBlockAndWait:(void (NS_NOESCAPE ^)(void))block  API_AVAILABLE(macosx(10.10),ios(8.0));

    
    
    
+ (nullable NSDictionary<NSString *, id> *)metadataForPersistentStoreOfType:(nullable NSString *)storeType URL:(NSURL *)url error:(NSError **)error API_DEPRECATED("Use -metadataForPersistentStoreOfType:URL:options:error: and pass in an options dictionary matching addPersistentStoreWithType", macosx(10.5,10.11), ios(3.0,9.0));

+ (BOOL)setMetadata:(nullable NSDictionary<NSString *, id> *)metadata forPersistentStoreOfType:(nullable NSString *)storeType URL:(NSURL*)url error:(NSError **)error API_DEPRECATED("Use  -setMetadata:forPersistentStoreOfType:URL:options:error: and pass in an options dictionary matching addPersistentStoreWithType", macosx(10.5,10.11), ios(3.0,9.0));


@end


    

NS_ASSUME_NONNULL_END

