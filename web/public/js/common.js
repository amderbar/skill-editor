function hashMap(hash, callback) {
    var new_hash = {};
    for (let key in hash) {
        if (hash.hasOwnProperty(key)) {
            new_hash[key] = callback(hash[key], key);
        }
    }
    return new_hash;
}